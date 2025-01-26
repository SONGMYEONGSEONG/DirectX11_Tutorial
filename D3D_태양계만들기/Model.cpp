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
	//T data = Matriex(��� �ڷ��� ������)
	vsConstantBuffer->data.world = mesh->GetTransformMatrix() * worldMatrix;
	vsConstantBuffer->data.viewProjection = viewProjectMatrix; //�� * ���� ���
	vsConstantBuffer->ApplyChanges();//constantbuffer�� ���data �޸� lock
	mesh->Draw();
}