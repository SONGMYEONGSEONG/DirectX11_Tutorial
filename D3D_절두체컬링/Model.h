//Mesh/Model
//쉽게 메시를 그리기위한 메시 객체 구조를 만든다.

#pragma once
#include "Mesh.h"
#include "ConstantBuffer.h"
class Model abstract
{
protected:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	std::unique_ptr<Mesh> mesh;
	ConstantBuffer<Matrices>* vsConstantBuffer;// vs : vertex Shader 

public:
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectMatrix);
};

