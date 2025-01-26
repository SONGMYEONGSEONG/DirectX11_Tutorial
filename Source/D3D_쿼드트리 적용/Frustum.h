//Frusturm Culling(����ü �ø�) = cliping(Ŭ����)
//ī�޶�(��) * ���� �����
// Frustum Culling (����ü �ø�) = cliping
//���� : clipspace �ۿ� ������ ���������� �ʴ°�
// 
//������� Frustum������ ���ϱ� ���Ͽ� ���� Frustum�� 8�� �������� ������ ���
//ī�޶�* ���� �����
//����ü �ø� ���൵
//0.���� Vertex(ī�޶� ȭ��) ���ϱ�(D3D �ϸ鼭 �����ϴ��� SpaceTransform(������ȯ))
//
//0 - 1. ���� Vertex = WorldVertex * ViewMatrix * ProjectionMatrix
////��������� ���ϴ°� ī�޶� ����ü ���ϱ� ���� ��(�⺻���ΰ�)
//
//���� vertex / (ViewMatrix * ProjectionMatrix)
//= WorldVertex * ViewMatrix * ProjectionMatrix / (WorldVertex * ViewMatrix * ProjectionMatrix)
//
//���� vertex * (1 / (ViewMatrix * ProjectionMatrix))
//= WorldVertex;
//
//�� ���� vertex* ���� ����� = vertex_world(������ ��ȯ���� ���� ��Ʈ����)
//
//���� ����� : �����(Inverse Matrix) : (ViewMatrix * ProjectionMatrix) ^ -1
//(���� �ܿ� �ʿ� x, D3D���� �Լ� ����)

#pragma once
#include "Components/DXCamera.h"

class Frustum
{
private:
	DXCamera* camera;

	float planeEpsilon; // ����׽�Ʈ���� ���Ǵ� �ε� �Ҽ��� ���������� ��Ÿ�� 
	XMVECTOR frustumPlane[6]; //Plane : ��� -> ����ü ��� 

public:
	bool Initialize(DXCamera* camera, float planeEpsilon = FLT_EPSILON);
	void ConstructFrustum();

	//bound : ������ ���ؼ� ��ü�� �������� ��Ʈ�ڽ�(������ ����) ���� �� �ȿ������� �ֵ��� ó�� üũ 

	bool IsInFrustum(XMFLOAT3 v); //Frustum �ȿ� ��ü Ȯ�� bool ����
	bool IsInFrustum(XMVECTOR v); //Frustum �ȿ� ��ü Ȯ�� bool ����
	bool IsInFrustumExceptUpDown(XMFLOAT3 v); //Frustum�� ����� �Ʒ����� ������ �ִ°ɷ� ����ϰ� �¿�յڸ� üũ�ϴ� ����

	bool IsInFrustumBoundsSphere(XMFLOAT3 v,float radius); //Frustum �ȿ� ��ü�� �������� �� ���� �ȿ� �ִ��� ��ü Ȯ�� bool ����
	bool IsInFrustumBoundsSphere(XMVECTOR v, float radius); //Frustum �ȿ� ��ü�� �������� �� ���� �ȿ� �ִ��� ��ü Ȯ�� bool ����
	bool IsInFrustumBoundsSphereExceptUpDown(XMFLOAT3 v, float radius); //Frustum�ȿ� ��ü�� �������� �� ���� ���� �Ʒ��¹����� �ִ°ɷ� ����ϰ� �¿�յڸ� üũ�ϴ� ����

	bool IsInFrustumBoundsCube(XMFLOAT3 v, float radius); //Frustum �ȿ� ����ü ��� �ȿ� ��ü Ȯ�� bool ����
	bool IsInFrustumBoundsCube(XMVECTOR v, float radius); //Frustum �ȿ� ����ü ��� �ȿ� ��ü Ȯ�� bool ����

	//Rectangle : AABB/OBB -> 2D���� ���鶧�ų� 3D������ UI ���鋚�� ���
	bool IsInFrustumBoundsRectangle(XMFLOAT3 v, XMFLOAT3 r); //Frustum �ȿ� 2D�簢�� �ȿ� ��ü Ȯ�� bool ����
	bool IsInFrustumBoundsRectangle(XMVECTOR v, XMVECTOR r); //Frustum �ȿ� 2D�簢�� �ȿ� ��ü Ȯ�� bool ����

private:
	bool IsInBoundsCube(XMVECTOR p, XMFLOAT3 v, XMFLOAT3 r);
};

