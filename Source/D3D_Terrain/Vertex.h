//Vertex.h
//사용자 정의 정점을 사용

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


/*Texture 클래스 작성후 추가 - 텍스처 맵핑을 위한 구조체 작성*/
//Vertex에 Texture 칠하기 : 점정에 텍스쳐 texCoord(uv) 추가 ,uv: uv 좌표계,Coord:좌표
//세이더를 이용하여 텍스처를 칠한다.
struct VertexTexture : VertexColor
{
	//Texutre x = u , y = v
	XMFLOAT2 texCoord; // Texture 들은 2D 이미지 이기때문에 2차원 벡터를 사용
	VertexTexture() : texCoord(0.0f, 0.0f) {}
	VertexTexture(float x, float y, float z, float r, float g, float b, float a, float u, float v) : VertexColor(x, y, z, r, g, b, a), texCoord(u, v) {}
	VertexTexture(float x, float y, float z, float r, float g, float b, float u, float v) : VertexTexture(x, y, z, r, g, b, 1.0f, u, v) {}
	VertexTexture(float x, float y, float z, float u, float v) : VertexTexture(x, y, z, 1.0f, 1.0f, 1.0f, 1.0f, u, v) {}

	// Texutre용 HLSL이 필요함 -> 새로운 쉐이더가 필요해짐
};

/*조명 HLSL 작성후 추가 */
//vertex의 법선(노말)베터
//빛으 ㅣ반사 벡터를 구하기 위해서는 법선벡터가 반드시 필요(게임수학 참고)
struct ModelVertex : VertexTexture
{
	XMFLOAT3 normal; //노말(법선)벡터
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
