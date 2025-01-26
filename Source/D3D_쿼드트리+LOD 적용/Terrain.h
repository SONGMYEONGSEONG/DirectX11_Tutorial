#pragma once
//#include <vector>
//#include "Model.h"
#include "QuadTree.h" // QuadTree �۾� �߰� 
// (2^n +1) Checker
#define PermitQuadTree(s) (0 == ((s - 1) & (s - 2))) // QuadTree �۾� �߰� 
class Terrain
{
private:
	int width, height; // �̹����� ��,�ʺ� 
	std::vector<ModelVertex> vertices;
	std::vector<DWORD> indices;

	VertexBuffer<ModelVertex> vertexBuffer;
	IndexBuffer indexBuffer;
	std::vector<Texture> textures;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ConstantBuffer<Matrices>* vsConstantBuffer;

	std::unique_ptr<QuadTree> quadTree; // QuadTree �۾� �߰� 

public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer, const std::string& filename, const std::string& texName,
		int numRepeat = 1);
	//void Draw(const XMMATRIX& viewProjectionMatrix);
	void Draw(const XMMATRIX& viewProjectionMatrix,Frustum* frustum = nullptr); // �������� QuadTree�� �̿��Ͽ� Ŭ����(����ü�ø�)�� �ϱ⶧���� Frustum�� ���ڰ����� �����;���
	void SetLOD(LOD* lod)
	{
		quadTree.get()->SetLOD(lod);
	}
private:
	bool LoadHeightMap(const std::string& filename, const int& numRepeat);
	void VerticesNormalVectorSetup(const DWORD& bottomLeftIndex, const DWORD& topLeftIndex, const DWORD& bottomRightIndex);
};

