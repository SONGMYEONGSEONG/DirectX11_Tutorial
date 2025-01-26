//2D이미지를 그리기 위한 Conponenet를 추가
#pragma once
#include "../DXGameObject.h"
#include "../Model.h"
class DXSpriteRenderer : public RenderComponent
{
private:
	std::unique_ptr<Mesh> mesh;
	ID3D11DeviceContext* deviceContext;
	ConstantBuffer<Matrices>* vsConstantBuffer;
public:
	DXSpriteRenderer(DXGameObject* owner);
	void LoadSpriteImage(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>&
		vsConstantBuffer, const std::string& filename);
protected:
	virtual void Opertate() override;
};