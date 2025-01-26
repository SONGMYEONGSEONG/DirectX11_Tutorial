//DXCamera.h
//view Matrix�� Projection Matirx�� �ʿ��ϱ� ������ ī�޶� ��ü�� �����.
//DirectX �������� ����� ī�޶� ��ü�� DXGameObject�� ������Ʈ�� ���
//DXGameObject / Components ���� ���� ������Ʈ�� �߰�
//DXTransform�� �̿��Ͽ� ���� �������� ���� ������ ī�޶� ���� ����

#pragma once
#include "../DXGameObject.h"
enum class ProjectionType { Perspective, Orthographic};
// Perspective(��������) : ��ǻ�� �׷��Ƚ� ������ word ���� ,���� ã�� 
// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=canny708&logNo=221547230049
// Orthographic(��������) : 
//https://ko.wikipedia.org/wiki/%EC%A0%95%EC%82%AC%EC%98%81

class DXCamera : public BaseComponent
{
private:
	ProjectionType type;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	float width, height;
	float nearZ, farZ; // Frustrm(����ü)���� ������ ���
	float fovDegrees; // ī�޶� ���� ���̴� ����

public:
	DXCamera(DXGameObject* owner);

	void SetProjectionType(ProjectionType type);
	void SetProjection(float width, float height, float nearZ, float farZ, float fovDefrees = 60.0f);
	void ChangeProjectionType(const ProjectionType& type);

	const ProjectionType& GetProjectionType() const { return type; }

	const XMMATRIX& GetViewMatrix() const { return viewMatrix; }
	const XMMATRIX& GetProjectionMatrix() const { return projectionMatrix; }

	XMMATRIX GetViewProjectionMatrix() const { return viewMatrix * projectionMatrix; }

	float GetDepth() const { return farZ; }

private:
	void UpdateMatrix();

	friend DXTransform;
};

