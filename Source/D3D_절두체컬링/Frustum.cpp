#include "Frustum.h"
bool Frustum::Initialize(DXCamera* camera, float planeEpsilon)
{
	if (!camera) return false;

	this->camera = camera;
	this->planeEpsilon = planeEpsilon;
	for (int i = 0; 6 > i; i++) frustumPlane[i] = XMVectorSet(0, 0, 0, 0);
	return true;
}

void Frustum::ConstructFrustum()
{
	//최종 vertex는 (-1,-1,0) ~ (1,1,1) 사이의 값으로 바뀐다.
	//최종Vertex = Vworld * ViewMatrix * ProjectMatirx해서 나온 Frusutm 영역의 8개 vertex data임.
	XMFLOAT3 frustumVertices[8] =
	{
		{-1.0f, -1.0f, 0.0f }, // near left bottom v0
		{1.0f, -1.0f, 0.0f }, // near right bottom v1
		{1.0f, -1.0f, 1.0f }, // far right bottom v2
		{-1.0f, -1.0f, 1.0f }, // far left bottom v3
		{-1.0f, 1.0f, 0.0f }, // near left top v4
		{1.0f, 1.0f, 0.0f }, // near right top v5
		{1.0f, 1.0f, 1.0f }, // far right top v6
		{-1.0f, 1.0f, 1.0f }, // far left top v7
	};

	//view * proj의 역행렬 계산.
	XMMATRIX invViewProjectionMatrix = XMMatrixInverse(NULL, camera->GetViewMatrix() * camera->GetProjectionMatrix());
	
	XMVECTOR frustumVectors[8]; //최종vertex * (viewprojectionMatrix)T를 계산한 vertex들의 vector
	for (int i = 0; 8 > i; i++)
	{
		// XMVector3TransformCoord : 주어진 행렬을 사용하여 벡터로 변형하는 함수
		//최종vertex * (viewprojectionMatrix)T 계산,
		frustumVectors[i] = XMVector3TransformCoord(XMVectorSet(frustumVertices[i].x, frustumVertices[i].y, frustumVertices[i].z, 1.0f),
			invViewProjectionMatrix);
	}

	//계산된 Vworld(월드상의 vertex)들을 3개씩 사용하여 
	frustumPlane[0] = XMPlaneFromPoints(frustumVectors[4], frustumVectors[7], frustumVectors[6]); // 상 평면(top)
	frustumPlane[1] = XMPlaneFromPoints(frustumVectors[0], frustumVectors[1], frustumVectors[2]); // 하 평면(bottom)
	frustumPlane[2] = XMPlaneFromPoints(frustumVectors[0], frustumVectors[4], frustumVectors[5]); // 근 평면(near)
	frustumPlane[3] = XMPlaneFromPoints(frustumVectors[2], frustumVectors[6], frustumVectors[7]); // 원 평면(far)
	frustumPlane[4] = XMPlaneFromPoints(frustumVectors[0], frustumVectors[3], frustumVectors[7]); // 좌 평면(left)
	frustumPlane[5] = XMPlaneFromPoints(frustumVectors[1], frustumVectors[5], frustumVectors[6]); // 우 평면(right)
}

bool Frustum::IsInFrustum(XMFLOAT3 v)
{
	return IsInFrustum(XMVectorSet(v.x, v.y, v.z, 1.0f));
}

bool Frustum::IsInFrustum(XMVECTOR v)
{
	return IsInFrustumBoundsSphere(v, 0.0f);
}

bool Frustum::IsInFrustumExceptUpDown(XMFLOAT3 v)
{
	return IsInFrustumBoundsSphereExceptUpDown(v, 0.0f);
}

bool Frustum::IsInFrustumBoundsSphere(XMFLOAT3 v, float radius)
{
	return IsInFrustumBoundsSphere(XMVectorSet(v.x, v.y, v.z, 1.f), radius);
}

bool Frustum::IsInFrustumBoundsSphere(XMVECTOR v, float radius)
{
	for (int i = 0; 6 > i; i++) // 왜 6인가? 절두체의 면이 6개임
	{
		//내적은 frustumPlane에서 v까지의 수직상 거리와 같다.(절두체 컬링 pdf 128페이지 참고)
		//벡터의 내적의 크기(벡터 * 법선벡터 = 해당면으로 수직의 벡터크기(길이) 
		// {벡터의 내적 < 오브젝트의 구 범위(반지름이 더 크면)} frustum안에 있다는 뜻이다.
		//XMPlaneDotCoord의 함수 반환값은 XMVECTOR인데, XMVECTOR의 모든값을 내적크기로 반환하기 떄문이다.
		//그래서 VectorGetX,Y,Z 함수 아무거나 사용해도 괜찮다. 전부 내적의 값이기 떄문이다.
		//평면방정식 내용 필요함, 참고해서 작성할것 
		if (XMVectorGetX(XMPlaneDotCoord(frustumPlane[i], v)) > (radius + planeEpsilon))
		{
			// 벡터의 내적(물체와 절두체 면의 수직상의 거리) > 물체의 경계(히트박스)거리 가 참인경우
			return false; //frustum 영역 외부에 있음
		}
	}

	return true;//frustum 영역 내부에 있음
}

bool Frustum:: IsInFrustumBoundsSphereExceptUpDown(XMFLOAT3 v, float radius)
{
	XMVECTOR vector = XMVectorSet(v.x, v.y, v.z, 1.0f);

	for (int i = 2; 6 > i ; i++) // 0,1 인덱스의 vertex는 윗면과 ,아랫면의 vertex이므로 제외
	{
		//XMPlaneDotCoord (평면(plane) , 벡터(vertex) ) : 평면과 벡터사이의 내적값을 반환.
		if (XMVectorGetX(XMPlaneDotCoord(frustumPlane[i], vector)) > (radius + planeEpsilon)) return false;
	}

	return true;
}

bool Frustum::IsInFrustumBoundsCube(XMFLOAT3 v, float radius)
{
	for (int i = 0; 6 > i; i++)
	{
		if (IsInBoundsCube(frustumPlane[i], v, XMFLOAT3(radius, radius, radius))) continue;
		return false;
	}
	return true;
}

bool Frustum::IsInFrustumBoundsCube(XMVECTOR v, float radius)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);
	return IsInFrustumBoundsCube(dest, radius);
}

bool Frustum::IsInFrustumBoundsRectangle(XMFLOAT3 v, XMFLOAT3 r)
{
	for (int i = 0; 6 > i; i++)
	{
		if (IsInBoundsCube(frustumPlane[i], v, r)) continue;
		return false;
	}
	return true;
}

bool Frustum::IsInFrustumBoundsRectangle(XMVECTOR v, XMVECTOR r)
{
	XMFLOAT3 destV, destR;
	XMStoreFloat3(&destV, v);
	XMStoreFloat3(&destR, r);
	return IsInFrustumBoundsRectangle(destV, destR);
}

bool Frustum::IsInBoundsCube(XMVECTOR p, XMFLOAT3 v, XMFLOAT3 r)
{
	return
		(
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x - r.x, v.y - r.y, v.z - r.z, 1.0f))) <= planeEpsilon) ||
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x - r.x, v.y - r.y, v.z + r.z, 1.0f))) <= planeEpsilon) ||
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x - r.x, v.y + r.y, v.z - r.z, 1.0f))) <= planeEpsilon) ||
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x - r.x, v.y + r.y, v.z + r.z, 1.0f))) <= planeEpsilon) ||
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x + r.x, v.y - r.y, v.z - r.z, 1.0f))) <= planeEpsilon) ||
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x + r.x, v.y - r.y, v.z + r.z, 1.0f))) <= planeEpsilon) ||
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x + r.x, v.y + r.y, v.z - r.z, 1.0f))) <= planeEpsilon) ||
			(XMVectorGetX(XMPlaneDotCoord(p, XMVectorSet(v.x + r.x, v.y + r.y, v.z + r.z, 1.0f))) <= planeEpsilon)
		);
}