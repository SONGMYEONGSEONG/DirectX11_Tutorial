//Constant.h
//Vertex Shader에 값을 입력하기 위한 구조체 헤더 -> 상수 구조체
//Shader는 상수 구조체의 메모리를 16바이트(중요) 단위로 사용한다.
//Shader의 bool 메모리 크기는 4바이트
#pragma once
#include<DirectXMath.h>
using namespace DirectX;
struct Matrices
{
	XMMATRIX world;//월드 매트릭스
	XMMATRIX viewProjection; //뷰 매트릭스 * 투영 매트릭스  
};

//조명 추가함과 작업
struct Material
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	float specularPower;
};
struct Light
{
	XMFLOAT4 camPosition; // 카메라의 위치(빛 위치)
	XMFLOAT4 light_color; // 빛의 색
	XMFLOAT4 light_direction; // 빛의 방향 
	XMFLOAT4 globalAmbient; //월드 환경광 -> material과 합쳐져서 유저한테 보여진다.
	Material material;

	//월드 환경광이 었어야 하는 이유 : matreail에서만 ambient를 갖고있는경우 빛이 없는 공간에서는 물체가 하나도 보이지 않기 때문에
	//즉, 엠비언트 계산시에 Material.ambient + globalambient = 최종적으로 보는 ambient인데 matreial이 빛이 없어 데이터가 0이여도
	//globalAMbient 값을 갖고있기에 어두운곳에서도 실루엣 형태로 비춰지는것이다.
};