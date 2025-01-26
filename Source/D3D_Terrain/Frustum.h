//Frusturm Culling(절두체 컬링) = cliping(클리핑)
//카메라(뷰) * 투영 역행렬
// Frustum Culling (절두체 컬링) = cliping
//목적 : clipspace 밖에 영역을 렌더링하지 않는것
// 
//월드상의 Frustum영역을 구하기 위하여 최종 Frustum의 8개 정점에서 역으로 계산
//카메라* 투영 역행렬
//절두체 컬링 진행도
//0.최종 Vertex(카메라 화면) 구하기(D3D 하면서 진행하던거 SpaceTransform(공간변환))
//
//0 - 1. 최종 Vertex = WorldVertex * ViewMatrix * ProjectionMatrix
////뷰투영행렬 곱하는건 카메라 절두체 구하기 위한 것(기본적인것)
//
//최종 vertex / (ViewMatrix * ProjectionMatrix)
//= WorldVertex * ViewMatrix * ProjectionMatrix / (WorldVertex * ViewMatrix * ProjectionMatrix)
//
//최종 vertex * (1 / (ViewMatrix * ProjectionMatrix))
//= WorldVertex;
//
//※ 최종 vertex* 투영 역행렬 = vertex_world(뷰투영 변환전의 월드 매트릭스)
//
//투영 역행렬 : 역행렬(Inverse Matrix) : (ViewMatrix * ProjectionMatrix) ^ -1
//(따로 외울 필요 x, D3D에서 함수 지원)

#pragma once
#include "Components/DXCamera.h"

class Frustum
{
private:
	DXCamera* camera;

	float planeEpsilon; // 평면테스트에서 사용되는 부동 소수점 오차범위를 나타냄 
	XMVECTOR frustumPlane[6]; //Plane : 평면 -> 절두체 평면 

public:
	bool Initialize(DXCamera* camera, float planeEpsilon = FLT_EPSILON);
	void ConstructFrustum();

	//bound : 간단히 말해서 물체의 기준으로 히트박스(도형은 자유) 만들어서 그 안에있으면 있따고 처리 체크 

	bool IsInFrustum(XMFLOAT3 v); //Frustum 안에 물체 확인 bool 변수
	bool IsInFrustum(XMVECTOR v); //Frustum 안에 물체 확인 bool 변수
	bool IsInFrustumExceptUpDown(XMFLOAT3 v); //Frustum의 윗면과 아랫면은 무조건 있는걸로 취급하고 좌우앞뒤만 체크하는 변수

	bool IsInFrustumBoundsSphere(XMFLOAT3 v,float radius); //Frustum 안에 물체의 기준으로 구 모형 안에 있는지 물체 확인 bool 변수
	bool IsInFrustumBoundsSphere(XMVECTOR v, float radius); //Frustum 안에 물체의 기준으로 구 모형 안에 있는지 물체 확인 bool 변수
	bool IsInFrustumBoundsSphereExceptUpDown(XMFLOAT3 v, float radius); //Frustum안에 물체의 기준으로 구 모혀 위와 아래는무조건 있는걸로 취급하고 좌우앞뒤만 체크하는 변수

	bool IsInFrustumBoundsCube(XMFLOAT3 v, float radius); //Frustum 안에 육면체 모양 안에 물체 확인 bool 변수
	bool IsInFrustumBoundsCube(XMVECTOR v, float radius); //Frustum 안에 육면체 모양 안에 물체 확인 bool 변수

	//Rectangle : AABB/OBB -> 2D게임 만들때거나 3D게임의 UI 만들떄도 사용
	bool IsInFrustumBoundsRectangle(XMFLOAT3 v, XMFLOAT3 r); //Frustum 안에 2D사각형 안에 물체 확인 bool 변수
	bool IsInFrustumBoundsRectangle(XMVECTOR v, XMVECTOR r); //Frustum 안에 2D사각형 안에 물체 확인 bool 변수

private:
	bool IsInBoundsCube(XMVECTOR p, XMFLOAT3 v, XMFLOAT3 r);
};

