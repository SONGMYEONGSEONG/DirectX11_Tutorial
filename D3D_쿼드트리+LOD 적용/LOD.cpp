#include "LOD.h"
bool LOD::Initialize(DXCamera* camera, int maxLevel)
{
	if (!camera) return false;

	cameraTransform = camera->GetGameObject()->GetTransform(); //사용하고있는 카메라의 TM행렬 초기화
	ratio = maxLevel / camera->GetDepth(); // LOD 레벨 조절을 위한 상수(비율) 초기화
	//LOD Level :  객체의 세부 수준을 정의하는 값
	// 카메라가 비추는 Z축(farZ)에서 LOD Level에서 지점을 상수로 지정 -> 세분화 하는 작업의 단위가 됨

	return true;
}

int LOD::GetInvLevel(XMFLOAT3 v)
{
	XMVECTOR vector = XMVectorSet(v.x, v.y, v.z, 1.0f); // 인자값으로 가져온 x,y,z값을 vector(x,y,z,w)로 Set

	//카메라의 위치 - vector(node의 이미지 중심위치를 인자값으로 가져옴) =  카메라와 노드의 중심 사이의 거리
	float distance = XMVectorGetX(XMVector3Length(cameraTransform->GetPosition() - vector));

	//해당 distance에 ratio를 곱해서 최종 LOD레벨을 결정함 
	//invlevel이라 하는 이유 : ratio = level / 거리(카메라 farZ) ,양변에 거리를 곱하면 
	//level = 거리(카메라 위치 - 노드의 이미지 center 벡터) * ratio(lod 레벨 상수) 
	//이 함수는 현재 노드 위치에 해당하는 레벨값을 구하는 공식이다.
	int invLevel = std::ceil(distance * ratio);//ceil : 올림연산 함수 

	
	return((1 > invLevel) ? 1 : invLevel);
}