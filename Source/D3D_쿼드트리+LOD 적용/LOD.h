//LOD.h
//예제를 위해서 따로 코드를 나눔
//처음부터 쿼드 트리와 합쳐서 만들어도 좋다.
#pragma once
#include "Components//DXCamera.h"
enum class Neighbor { Up =0,Down,Left,Right};//해당 노드에서 이어지는 Node 방향을 나타냄
class LOD // Level of Detail
{
private:
	float ratio; // LOD 레벨 조절을 위한 상수(비율) = MaxLevel(LODLevel의 최대값) / 카메라의 Z축(깊이) 
	DXTransform* cameraTransform; // Camera의 TM행렬(크기행렬,회전행렬,위치행렬 등등)

public:
	// LOD는 카메라 거리로 폴리곤 수(vertex수)를 제어하는것이기에 camera 객체를 가져와야함
	bool Initialize(DXCamera* camera, int maxLevel = 200);
	int GetInvLevel(XMFLOAT3 v); // 레벨의 역수 get 함수 : 
};

