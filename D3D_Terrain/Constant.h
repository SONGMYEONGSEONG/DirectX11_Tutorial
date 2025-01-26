//Constant.h
//Vertex Shader�� ���� �Է��ϱ� ���� ����ü ��� -> ��� ����ü
//Shader�� ��� ����ü�� �޸𸮸� 16����Ʈ(�߿�) ������ ����Ѵ�.
//Shader�� bool �޸� ũ��� 4����Ʈ
#pragma once
#include<DirectXMath.h>
using namespace DirectX;
struct Matrices
{
	XMMATRIX world;//���� ��Ʈ����
	XMMATRIX viewProjection; //�� ��Ʈ���� * ���� ��Ʈ����  
};

//���� �߰��԰� �۾�
struct Material
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	float specularPower;
};
struct Light
{
	XMFLOAT4 camPosition; // ī�޶��� ��ġ(�� ��ġ)
	XMFLOAT4 light_color; // ���� ��
	XMFLOAT4 light_direction; // ���� ���� 
	XMFLOAT4 globalAmbient; //���� ȯ�汤 -> material�� �������� �������� ��������.
	Material material;

	//���� ȯ�汤�� ����� �ϴ� ���� : matreail������ ambient�� �����ִ°�� ���� ���� ���������� ��ü�� �ϳ��� ������ �ʱ� ������
	//��, �����Ʈ ���ÿ� Material.ambient + globalambient = ���������� ���� ambient�ε� matreial�� ���� ���� �����Ͱ� 0�̿���
	//globalAMbient ���� �����ֱ⿡ ��ο�������� �Ƿ翧 ���·� �������°��̴�.
};