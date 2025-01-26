#pragma once
#include <vector>
#include "Model.h"
class Terrain
{
private:
	int width, height;
	std::vector<ModelVertex> vertices;
	std::vector<DWORD> indices;

	VertexBuffer<ModelVertex> vertexBuffer;
	IndexBuffer indexBuffer;
	std::vector<Texture> textures;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ConstantBuffer<Matrices>* vsConstantBuffer;

public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer, const std::string& filename, const std::string& texName,
		int numRepeat = 1);
	void Draw(const XMMATRIX& viewProjectionMatrix);

private:
	bool LoadHeightMap(const std::string& filename, const int& numRepeat);
	void VerticesNormalVectorSetup(const DWORD& bottomLeftIndex, const DWORD& topLeftIndex, const DWORD& bottomRightIndex);
};

