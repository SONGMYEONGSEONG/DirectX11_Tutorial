#pragma once
#include "BaseComponent.h"
#include <set>
#include <DirectXMath.h>
using namespace DirectX;

#define Rad2Deg 57.2957795130f
#define Deg2Rad 0.0174532925f

namespace VECTOR3
{
	const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR down = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	const XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR back = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR left = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
}

class DXGameObject;
class DXTransform : public BaseComponent
{
private:
	DXTransform* parent;
	std::set<DXTransform*> childs;

	XMFLOAT3 position; //위치
	XMFLOAT3 rotation; //회전
	XMFLOAT3 scale; //크기 

	XMMATRIX translationMatrix; //이동행렬
	XMMATRIX rotationMatrix; // 회전행렬
	XMMATRIX scaleMatrix; //크기행렬

	XMMATRIX worldMatrix; // 월드행렬

	XMVECTOR up; // y축
	XMVECTOR forward; // z축
	XMVECTOR right; // x축

public:
	DXTransform(DXGameObject* gameObject);

	const DXGameObject* GameObject() const;

	void SetParent(DXTransform* parent);

	void SetLookAt(XMFLOAT3 lookAtPos);

	void SetPosition(const XMFLOAT3& position);
	void SetLocalRotation(const XMFLOAT3& rotation);
	void SetLocalScale(const XMFLOAT3& scale);

	void AdjustPosition(const XMFLOAT3& value);
	void AdjustPosition(const XMVECTOR& value);
	void AdjustRotation(const XMFLOAT3& value);
	void AdjustRotation(const XMVECTOR& value);

	const XMVECTOR& GetPosition() const;
	const XMVECTOR& GetScale() const;

	const XMMATRIX& GetTranslationMatrix() const;
	const XMMATRIX& GetRotationMatrix() const;
	const XMMATRIX& GetScaleMatrix() const;
	const XMMATRIX& GetWorldMatrix() const;

	const XMVECTOR& GetUp() const;
	const XMVECTOR& GetForward() const;
	const XMVECTOR& GetRight() const;

private:
	void UpdateTranslationMatrix();
	void UpdateRotationMatrix();
	void UpdateScaleMatrix();
	void UpdateWorldMatrix();

	void AddChild(DXTransform* child);
	void RemoveChild(DXTransform* child);
};