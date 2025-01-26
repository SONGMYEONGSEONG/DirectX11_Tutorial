//DXGameObject�� ������Ʈ�� ���

#pragma once
#include "../DXGameObject.h"
#include "../Model.h"

class DXMeshRenderer : public RenderComponent
{
private:
	Model* model;

public:
	DXMeshRenderer(DXGameObject* owner);
	void SetModel(Model* model);

protected:
	virtual void Opertate() override;

};

