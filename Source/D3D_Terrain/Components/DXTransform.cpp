//DXTransForm 수정 ( DXCamera 작성후 수정)
//해당 DXGameObject가 카메라일 경우, DXTransform의 이동,회전 행렬갱신에 맞게 카메라의 행렬도 같이 갱신

#include "../DXGameObject.h"


DXTransform::DXTransform(DXGameObject* gameObject) :
	BaseComponent(gameObject),
	parent(nullptr),
	position(0.0f, 0.0f, 0.0f),
	rotation(0.0f, 0.0f, 0.0f),
	scale(1.0f, 1.0f, 1.0f)
{
	translationMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();
	scaleMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixIdentity();

	up = VECTOR3::up;
	forward = VECTOR3::forward;
	right = VECTOR3::right;
}

void DXTransform::SetParent(DXTransform* parent)
{
	if ((nullptr != this->parent) && (this->parent != parent))
	{
		this->parent->RemoveChild(this);
	}
	if (nullptr != parent)
	{
		parent->AddChild(this);
	}
	this->parent = parent;

	UpdateWorldMatrix();
}

void DXTransform::SetLookAt(XMFLOAT3 lookAtPos)
{
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, GetPosition());

	if (lookAtPos.x == pos.x &&
		lookAtPos.y == pos.y &&
		lookAtPos.z == pos.z) return;

	lookAtPos.x = pos.x - lookAtPos.x;
	lookAtPos.y = pos.y - lookAtPos.y;
	lookAtPos.z = pos.z - lookAtPos.z;

	float pitch = 0.0f; // x축 회전
	if (0.0f != lookAtPos.y)
	{
		//dist : distriction 거리 = sqrt(루트) ( x^2 + z^2) (피타고라스, 두점 사이의거리를 구하는공식)
		const float dist = sqrt((lookAtPos.x * lookAtPos.x) + (lookAtPos.z * lookAtPos.z));
		if (0.000001f <= dist) pitch = atan(lookAtPos.y / dist);//역탄젠트 계산(아크탄젠트)
	}

	float yaw = 0.0f; // y축 회전
	if (0.0f != lookAtPos.x) yaw = atan(lookAtPos.x / lookAtPos.z);
	if (lookAtPos.z > 0.0f) yaw += XM_PI;

	SetLocalRotation(XMFLOAT3(pitch * Rad2Deg, yaw * Rad2Deg, 0.0f));
}

void DXTransform::SetPosition(const XMFLOAT3& position)
{
	this->position = position;
	UpdateTranslationMatrix();
}

void DXTransform::SetLocalRotation(const XMFLOAT3& rotation)
{
	this->rotation = rotation;
	UpdateRotationMatrix();
}

void DXTransform::SetLocalScale(const XMFLOAT3& scale)
{
	this->scale = scale;
	UpdateScaleMatrix();
}

void DXTransform::AdjustPosition(const XMFLOAT3& value)
{
	SetPosition({ position.x + value.x, position.y + value.y, position.z + value.z });
}

void DXTransform::AdjustPosition(const XMVECTOR& value)
{
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, value);
	AdjustPosition(pos);
}

void DXTransform::AdjustRotation(const XMFLOAT3& value)
{
	SetLocalRotation({ rotation.x + value.x, rotation.y + value.y, rotation.z + value.z });
}

void DXTransform::AdjustRotation(const XMVECTOR& value)
{
	XMFLOAT3 rot;
	XMStoreFloat3(&rot, value);
	AdjustRotation(rot);
}

const XMVECTOR& DXTransform::GetPosition() const
{
	return worldMatrix.r[3];
}

const XMVECTOR& DXTransform::GetScale() const
{
	XMVECTOR s = XMVectorSet(scale.x, scale.y, scale.z, 1.0f);
	if (parent) s = s * parent->GetScale();
	return s;
}

const XMMATRIX& DXTransform::GetTranslationMatrix() const
{
	return translationMatrix;
}

const XMMATRIX& DXTransform::GetRotationMatrix() const
{
	return rotationMatrix;
}

const XMMATRIX& DXTransform::GetScaleMatrix() const
{
	return scaleMatrix;
}

const XMMATRIX& DXTransform::GetWorldMatrix() const
{
	return worldMatrix;
}

const DXGameObject* DXTransform::GameObject() const
{
	return owner;
}

const XMVECTOR& DXTransform::GetUp() const
{
	return up;
}

const XMVECTOR& DXTransform::GetForward() const
{
	return forward;
}

const XMVECTOR& DXTransform::GetRight() const
{
	return right;
}

#include "../Components/DXCamera.h"

void DXTransform::UpdateTranslationMatrix()
{
	//현재 위치값에 따른 위치행렬이 구성됨
	translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	UpdateWorldMatrix();

	// TODO: Camera Matrix Update
	/*DXCamera 작성후 추가*/
	DXCamera* cam = owner->GetComponent<DXCamera>();
	if (cam) cam->UpdateMatrix();
	/**/
	for (DXTransform* chlid : childs) chlid->UpdateTranslationMatrix();
}

void DXTransform::UpdateRotationMatrix()
{
	rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x * Deg2Rad, rotation.y * Deg2Rad, rotation.z * Deg2Rad);
	up = XMVector3TransformCoord(VECTOR3::up, rotationMatrix);
	forward = XMVector3TransformCoord(VECTOR3::forward, rotationMatrix);
	right = XMVector3TransformCoord(VECTOR3::right, rotationMatrix);

	UpdateWorldMatrix();

	// TODO: Camera Matrix Update
	/*DXCamera 작성후 추가*/
	DXCamera* cam = owner->GetComponent<DXCamera>();
	if (cam) cam->UpdateMatrix();
	/**/
	for (DXTransform* chlid : childs) chlid->UpdateRotationMatrix();
}

void DXTransform::UpdateScaleMatrix()
{
	scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

	UpdateWorldMatrix();

	for (DXTransform* chlid : childs) chlid->UpdateScaleMatrix();
}

void DXTransform::UpdateWorldMatrix()
{
	//행렬의 곱은 각 원소의 합이다.
	//행렬의 곱은 순서가 매우 중요하다.
	// 스(스케일) * 자(자전,회전) * 이(이동) 순서 중요, 
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	//계층구조 
	if (parent)
	{
		worldMatrix = worldMatrix * parent->worldMatrix;//월드매트릭스를 구한후 계층구조일경우 부모의 월드행렬을 가져옴 
	}
}

void DXTransform::AddChild(DXTransform* child)
{
	childs.insert(child);
}

void DXTransform::RemoveChild(DXTransform* child)
{
	auto iter = childs.find(child);
	if(childs.end() != iter) childs.erase(iter);
}
