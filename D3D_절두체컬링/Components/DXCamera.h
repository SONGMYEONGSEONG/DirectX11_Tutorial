//DXCamera.h
//view Matrix와 Projection Matirx가 필요하기 때문에 카메라 객체를 만든다.
//DirectX 렌더링에 사용할 카메라 객체를 DXGameObject의 컴포넌트로 사용
//DXGameObject / Components 폴더 전부 프로젝트에 추가
//DXTransform을 이용하여 쉽게 움직임이 제어 가능한 카메라 구현 가능

#pragma once
#include "../DXGameObject.h"
enum class ProjectionType { Perspective, Orthographic};
// Perspective(원근투영) : 컴퓨터 그래픽스 정리한 word 파일 ,투영 찾기 
// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=canny708&logNo=221547230049
// Orthographic(직교투영) : 
//https://ko.wikipedia.org/wiki/%EC%A0%95%EC%82%AC%EC%98%81

class DXCamera : public BaseComponent
{
private:
	ProjectionType type;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	float width, height;
	float nearZ, farZ; // Frustrm(절두체)에서 나오는 용어
	float fovDegrees; // 카메라 시점 보이는 각도

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

