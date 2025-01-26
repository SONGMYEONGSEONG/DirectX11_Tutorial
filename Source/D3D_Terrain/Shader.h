//Shaderh.h
//Direct Shader
//DriectX 11에서 고정파이프라인이 폐지되어 , 
//아무리 간단한 폴리곤을 렌더링 하더라도 반드시 셰이더를 거쳐야만한다.

#pragma once
#pragma comment(lib,"d3dcompiler.lib")
#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Log.h"

class VertexShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader; //정점 쉐이더

public:
	//ID3D11Device : 디비이스는 리소스를 만들고 디스플레이 어댑터의 기능을 열거하는데 사용 
	//각 애플리케이션에는 디바이스가 하나 이상 있어야 하며, 대부분의 애플리케이션은 하나의 디바이스만 만듭니다.
	bool Initialize(ID3D11Device* pDevice, std::wstring fileName, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);
	ID3D11VertexShader* GetShader() const { return vertexShader.Get(); }
	ID3D11InputLayout* GetInputLayOut() const { return inputLayout.Get(); }
};

class PixelShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

public:
	bool Initialize(ID3D11Device* pDevice, std::wstring fileName);
	ID3D11PixelShader* GetShader() const { return pixelShader.Get(); }
};
