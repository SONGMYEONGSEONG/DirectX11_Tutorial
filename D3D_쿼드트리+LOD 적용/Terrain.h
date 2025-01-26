#pragma once
//#include <vector>
//#include "Model.h"
#include "QuadTree.h" // QuadTree 작업 추가 
// (2^n +1) Checker
#define PermitQuadTree(s) (0 == ((s - 1) & (s - 2))) // QuadTree 작업 추가 
class Terrain
{
private:
	int width, height; // 이미지의 폭,너비 
	std::vector<ModelVertex> vertices;
	std::vector<DWORD> indices;

	VertexBuffer<ModelVertex> vertexBuffer;
	IndexBuffer indexBuffer;
	std::vector<Texture> textures;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ConstantBuffer<Matrices>* vsConstantBuffer;

	std::unique_ptr<QuadTree> quadTree; // QuadTree 작업 추가 

public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer, const std::string& filename, const std::string& texName,
		int numRepeat = 1);
	//void Draw(const XMMATRIX& viewProjectionMatrix);
	void Draw(const XMMATRIX& viewProjectionMatrix,Frustum* frustum = nullptr); // 렌더링시 QuadTree를 이용하여 클리핑(절두체컬링)을 하기때문에 Frustum을 인자값으로 가져와야함
	void SetLOD(LOD* lod)
	{
		quadTree.get()->SetLOD(lod);
	}
private:
	bool LoadHeightMap(const std::string& filename, const int& numRepeat);
	void VerticesNormalVectorSetup(const DWORD& bottomLeftIndex, const DWORD& topLeftIndex, const DWORD& bottomRightIndex);
};

