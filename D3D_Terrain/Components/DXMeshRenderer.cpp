#include "DXMeshRenderer.h"
DXMeshRenderer::DXMeshRenderer(DXGameObject* owner) : RenderComponent(owner), model(nullptr) {}
void DXMeshRenderer::SetModel(Model* model) { this->model = model; }
void DXMeshRenderer::Opertate()
{
	if (model) model->Draw(owner->GetTransform()->GetWorldMatrix(), viewProjectionMatrix);
}