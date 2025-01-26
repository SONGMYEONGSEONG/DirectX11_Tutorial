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
	//���� vertex�� (-1,-1,0) ~ (1,1,1) ������ ������ �ٲ��.
	//����Vertex = Vworld * ViewMatrix * ProjectMatirx�ؼ� ���� Frusutm ������ 8�� vertex data��.
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

	//view * proj�� ����� ���.
	XMMATRIX invViewProjectionMatrix = XMMatrixInverse(NULL, camera->GetViewMatrix() * camera->GetProjectionMatrix());
	
	XMVECTOR frustumVectors[8]; //����vertex * (viewprojectionMatrix)T�� ����� vertex���� vector
	for (int i = 0; 8 > i; i++)
	{
		// XMVector3TransformCoord : �־��� ����� ����Ͽ� ���ͷ� �����ϴ� �Լ�
		//����vertex * (viewprojectionMatrix)T ���,
		frustumVectors[i] = XMVector3TransformCoord(XMVectorSet(frustumVertices[i].x, frustumVertices[i].y, frustumVertices[i].z, 1.0f),
			invViewProjectionMatrix);
	}

	//���� Vworld(������� vertex)���� 3���� ����Ͽ� 
	frustumPlane[0] = XMPlaneFromPoints(frustumVectors[4], frustumVectors[7], frustumVectors[6]); // �� ���(top)
	frustumPlane[1] = XMPlaneFromPoints(frustumVectors[0], frustumVectors[1], frustumVectors[2]); // �� ���(bottom)
	frustumPlane[2] = XMPlaneFromPoints(frustumVectors[0], frustumVectors[4], frustumVectors[5]); // �� ���(near)
	frustumPlane[3] = XMPlaneFromPoints(frustumVectors[2], frustumVectors[6], frustumVectors[7]); // �� ���(far)
	frustumPlane[4] = XMPlaneFromPoints(frustumVectors[0], frustumVectors[3], frustumVectors[7]); // �� ���(left)
	frustumPlane[5] = XMPlaneFromPoints(frustumVectors[1], frustumVectors[5], frustumVectors[6]); // �� ���(right)
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
	for (int i = 0; 6 > i; i++) // �� 6�ΰ�? ����ü�� ���� 6����
	{
		//������ frustumPlane���� v������ ������ �Ÿ��� ����.(����ü �ø� pdf 128������ ����)
		//������ ������ ũ��(���� * �������� = �ش������ ������ ����ũ��(����) 
		// {������ ���� < ������Ʈ�� �� ����(�������� �� ũ��)} frustum�ȿ� �ִٴ� ���̴�.
		//XMPlaneDotCoord�� �Լ� ��ȯ���� XMVECTOR�ε�, XMVECTOR�� ��簪�� ����ũ��� ��ȯ�ϱ� �����̴�.
		//�׷��� VectorGetX,Y,Z �Լ� �ƹ��ų� ����ص� ������. ���� ������ ���̱� �����̴�.
		//�������� ���� �ʿ���, �����ؼ� �ۼ��Ұ� 
		if (XMVectorGetX(XMPlaneDotCoord(frustumPlane[i], v)) > (radius + planeEpsilon))
		{
			// ������ ����(��ü�� ����ü ���� �������� �Ÿ�) > ��ü�� ���(��Ʈ�ڽ�)�Ÿ� �� ���ΰ��
			return false; //frustum ���� �ܺο� ����
		}
	}

	return true;//frustum ���� ���ο� ����
}

bool Frustum:: IsInFrustumBoundsSphereExceptUpDown(XMFLOAT3 v, float radius)
{
	XMVECTOR vector = XMVectorSet(v.x, v.y, v.z, 1.0f);

	for (int i = 2; 6 > i ; i++) // 0,1 �ε����� vertex�� ����� ,�Ʒ����� vertex�̹Ƿ� ����
	{
		//XMPlaneDotCoord (���(plane) , ����(vertex) ) : ���� ���ͻ����� �������� ��ȯ.
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