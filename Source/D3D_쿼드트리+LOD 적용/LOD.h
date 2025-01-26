//LOD.h
//������ ���ؼ� ���� �ڵ带 ����
//ó������ ���� Ʈ���� ���ļ� ���� ����.
#pragma once
#include "Components//DXCamera.h"
enum class Neighbor { Up =0,Down,Left,Right};//�ش� ��忡�� �̾����� Node ������ ��Ÿ��
class LOD // Level of Detail
{
private:
	float ratio; // LOD ���� ������ ���� ���(����) = MaxLevel(LODLevel�� �ִ밪) / ī�޶��� Z��(����) 
	DXTransform* cameraTransform; // Camera�� TM���(ũ�����,ȸ�����,��ġ��� ���)

public:
	// LOD�� ī�޶� �Ÿ��� ������ ��(vertex��)�� �����ϴ°��̱⿡ camera ��ü�� �����;���
	bool Initialize(DXCamera* camera, int maxLevel = 200);
	int GetInvLevel(XMFLOAT3 v); // ������ ���� get �Լ� : 
};

