//Vertex.h
//����� ���� ������ ���

#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 pos;

	Vertex() : pos(0.0f, 0.0f, 0.0f) {}
	Vertex(float x, float y, float z) : pos(x, y, z) {}

};

struct VertexColor : Vertex
{
	XMFLOAT4 color;

	VertexColor() : color(1.0f, 1.0f, 1.0f, 1.0f) {}
	VertexColor(float x, float y, float z, float r, float g, float b, float a) : Vertex(x, y, z), color(r, g, b, a) {}
	VertexColor(float x, float y, float z, float r, float g, float b) : VertexColor(x, y, z, r, g, b, 1.0f) {}
};


/*Texture Ŭ���� �ۼ��� �߰� - �ؽ�ó ������ ���� ����ü �ۼ�*/
//Vertex�� Texture ĥ�ϱ� : ������ �ؽ��� texCoord(uv) �߰� ,uv: uv ��ǥ��,Coord:��ǥ
//���̴��� �̿��Ͽ� �ؽ�ó�� ĥ�Ѵ�.
struct VertexTexture : VertexColor
{
	//Texutre x = u , y = v
	XMFLOAT2 texCoord; // Texture ���� 2D �̹��� �̱⶧���� 2���� ���͸� ���
	VertexTexture() : texCoord(0.0f, 0.0f) {}
	VertexTexture(float x, float y, float z, float r, float g, float b, float a, float u, float v) : VertexColor(x, y, z, r, g, b, a), texCoord(u, v) {}
	VertexTexture(float x, float y, float z, float r, float g, float b, float u, float v) : VertexTexture(x, y, z, r, g, b, 1.0f, u, v) {}
	VertexTexture(float x, float y, float z, float u, float v) : VertexTexture(x, y, z, 1.0f, 1.0f, 1.0f, 1.0f, u, v) {}

	// Texutre�� HLSL�� �ʿ��� -> ���ο� ���̴��� �ʿ�����
};

/*���� HLSL �ۼ��� �߰� */
//vertex�� ����(�븻)����
//���� �ӹݻ� ���͸� ���ϱ� ���ؼ��� �������Ͱ� �ݵ�� �ʿ�(���Ӽ��� ����)
struct ModelVertex : VertexTexture
{
	XMFLOAT3 normal; //�븻(����)����
	ModelVertex() : normal(0.0f, 0.0f, 0.0f) {}
	ModelVertex(float x, float y, float z, float r, float g, float b, float a, float u, float v, float nx, float ny, float nz)
		:VertexTexture(x, y, z, r, g, b, a, u, v), normal(nx, ny, nz)
	{
		XMVECTOR normalVector = XMVectorSet(normal.x, normal.y, normal.z, 0.0f);
		XMStoreFloat3(&normal,XMVector3Normalize(normalVector));
	}
	ModelVertex(float x, float y, float z, float r, float g, float b, float u, float v, float nx, float ny, float nz)
		: ModelVertex(x, y, z, r, g, b, 1.0f, u, v, nx, ny, nz) {}
	ModelVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		: ModelVertex(x, y, z, 1.0f, 1.0f, 1.0f, 1.0f, u, v, nx, ny, nz) {}
	ModelVertex(float x, float y, float z, float u, float v) : ModelVertex(x, y, z, 1.0f, 1.0f, 1.0f, 1.0f, u, v, x, y, z) {}
};
