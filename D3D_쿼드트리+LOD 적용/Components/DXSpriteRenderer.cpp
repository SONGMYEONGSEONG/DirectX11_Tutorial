#include "DXSpriteRenderer.h"
DXSpriteRenderer::DXSpriteRenderer(DXGameObject* owner) :
	RenderComponent(owner),
	deviceContext(nullptr),
	vsConstantBuffer(nullptr)
{
}
void DXSpriteRenderer::LoadSpriteImage(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer, const std::string& filename)
{
	this->deviceContext = deviceContext;
	this->vsConstantBuffer = &vsConstantBuffer;

	std::vector<ModelVertex> vertices =
	{
		//vertex 좌표 이동시에 원점이 중앙이다. 여기서 원점 세팅된게 아님 
		ModelVertex(-0.5f, -0.5f, 0.0f,0.0f, 0.0f), // top left
		ModelVertex(0.5f, -0.5f, 0.0f,1.0f, 0.0f), // top right
		ModelVertex(-0.5f, 0.5f, 0.0f,0.0f, 1.0f), // bottom left
		ModelVertex(0.5f, 0.5f, 0.0f,1.0f, 1.0f), // bottom right
	};
	std::vector<DWORD> indices =
	{
		0, 1, 2,
		2, 1, 3
	};
	std::vector<Texture> textures = { Texture(device, filename) };
	mesh = std::make_unique<Mesh>(device, deviceContext, vertices, indices, textures, XMMatrixIdentity());
}
void DXSpriteRenderer::Opertate()
{
	deviceContext->VSSetConstantBuffers(0, 1, vsConstantBuffer->GetAddressOf());
	vsConstantBuffer->data.world = mesh->GetTransformMatrix() * owner->GetTransform()->GetWorldMatrix();
	vsConstantBuffer->data.viewProjection = viewProjectionMatrix;
	vsConstantBuffer->ApplyChanges();
	mesh->Draw();
}
