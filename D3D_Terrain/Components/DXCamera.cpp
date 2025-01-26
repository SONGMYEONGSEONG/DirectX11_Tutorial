#include "../Components/DXCamera.h"
DXCamera::DXCamera(DXGameObject* owner) :
	BaseComponent(owner),
	type(ProjectionType::Perspective)
{
	UpdateMatrix();
}

void DXCamera::SetProjectionType(ProjectionType type)
{
	this->type = type;

	UpdateMatrix();
}

void DXCamera::SetProjection(float width, float height, float nearZ, float farZ, float fovDegrees)
{
	this->width = width;
	this->height = height;
	this->nearZ = nearZ;
	this->farZ = farZ;
	this->fovDegrees = fovDegrees;

	switch (type)
	{ // 원근/직교(Perspective/Orthographic) 투영 행렬 생성.
	case ProjectionType::Perspective: projectionMatrix = XMMatrixPerspectiveFovLH(fovDegrees * Deg2Rad, (width / height), nearZ, farZ); break;
	case ProjectionType::Orthographic: projectionMatrix = XMMatrixOrthographicOffCenterLH(-width / 2, width / 2, height / 2, -height / 2, nearZ, farZ); break;
	}
}

void DXCamera::ChangeProjectionType(const ProjectionType& type)
{
	if (type == this->type) return;
	SetProjectionType(type);
	SetProjection(width, height, nearZ, farZ, fovDegrees);
}

void DXCamera::UpdateMatrix()
{
	DXTransform* tr = owner->GetTransform();
	XMVECTOR position = tr->GetPosition();
	switch (type)
	{
		case ProjectionType::Perspective:
		{
			XMVECTOR lookAt = tr->GetForward();
			lookAt += position;
			XMVECTOR upVector = tr->GetUp();
			viewMatrix = XMMatrixLookAtLH(position, lookAt, upVector);
			break;
		}
		case ProjectionType::Orthographic:
		{
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, position);
			XMMATRIX translationOffsetMatrix = XMMatrixTranslation(-pos.x, -pos.y, 0.0f);
			viewMatrix = tr->GetRotationMatrix() * translationOffsetMatrix;
			break;
		}
	}
}