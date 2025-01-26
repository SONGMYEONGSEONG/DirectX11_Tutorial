#include "LOD.h"
bool LOD::Initialize(DXCamera* camera, int maxLevel)
{
	if (!camera) return false;

	cameraTransform = camera->GetGameObject()->GetTransform(); //����ϰ��ִ� ī�޶��� TM��� �ʱ�ȭ
	ratio = maxLevel / camera->GetDepth(); // LOD ���� ������ ���� ���(����) �ʱ�ȭ
	//LOD Level :  ��ü�� ���� ������ �����ϴ� ��
	// ī�޶� ���ߴ� Z��(farZ)���� LOD Level���� ������ ����� ���� -> ����ȭ �ϴ� �۾��� ������ ��

	return true;
}

int LOD::GetInvLevel(XMFLOAT3 v)
{
	XMVECTOR vector = XMVectorSet(v.x, v.y, v.z, 1.0f); // ���ڰ����� ������ x,y,z���� vector(x,y,z,w)�� Set

	//ī�޶��� ��ġ - vector(node�� �̹��� �߽���ġ�� ���ڰ����� ������) =  ī�޶�� ����� �߽� ������ �Ÿ�
	float distance = XMVectorGetX(XMVector3Length(cameraTransform->GetPosition() - vector));

	//�ش� distance�� ratio�� ���ؼ� ���� LOD������ ������ 
	//invlevel�̶� �ϴ� ���� : ratio = level / �Ÿ�(ī�޶� farZ) ,�纯�� �Ÿ��� ���ϸ� 
	//level = �Ÿ�(ī�޶� ��ġ - ����� �̹��� center ����) * ratio(lod ���� ���) 
	//�� �Լ��� ���� ��� ��ġ�� �ش��ϴ� �������� ���ϴ� �����̴�.
	int invLevel = std::ceil(distance * ratio);//ceil : �ø����� �Լ� 

	
	return((1 > invLevel) ? 1 : invLevel);
}