#include "Model.h"

void Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->vsConstantBuffer = &vsConstantBuffer;
}

void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectMatrix)
{
	deviceContext->VSSetConstantBuffers(0, 1, vsConstantBuffer->GetAddressOf());
	//T data = Matriex(행렬 자료형 들어가있음)
	vsConstantBuffer->data.world = mesh->GetTransformMatrix() * worldMatrix;
	vsConstantBuffer->data.viewProjection = viewProjectMatrix; //뷰 * 투영 행렬
	vsConstantBuffer->ApplyChanges();//constantbuffer의 상수data 메모리 lock
	mesh->Draw();
}