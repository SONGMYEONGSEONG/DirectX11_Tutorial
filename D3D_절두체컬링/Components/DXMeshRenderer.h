//DXGameObject의 컴포넌트로 사용

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

