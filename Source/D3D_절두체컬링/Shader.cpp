//Shader.cpp
#include "Shader.h"

//Mesh : 다면체의 형태를 구성하는 폴리곤의 집합 
//ID3D10Blob :
// 1.Mesh(메쉬)  최적화 및 로드 작업 중에 vertex(정점), 인접성 및 material(재질) 정보를 저장하는 데이터 버퍼
// 2.vertex,Geomerty Shader(기하 도형셰이더) 및 VS(vertex Shader)를 컴파일하는 API에서 개체 코드 및 오류 메시지를 반환
// Blob은 모든 다이렉트 3D 버전의 코드로 사용가능 

bool VertexShader::Initialize(ID3D11Device* pDevice, std::wstring fileName, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	Microsoft::WRL::ComPtr<ID3D10Blob>errorBlob, vertexBuffer;

    /*
	D3DCompileFromFile(_In_ LPCWSTR pFileName,
    _In_reads_opt_(_Inexpressible_(pDefines->Name != NULL)) CONST D3D_SHADER_MACRO* pDefines,
        _In_opt_ ID3DInclude* pInclude,
        _In_ LPCSTR pEntrypoint,
        _In_ LPCSTR pTarget,
        _In_ UINT Flags1,
        _In_ UINT Flags2,
        _Out_ ID3DBlob** ppCode,
        _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorMsgs);
     */

    //정점 쉐이더 컴파일
    //vs_5_0(vertex shader 5.0) DirectX 11에서는 5.0버전을 사용
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		vertexBuffer.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to compile Shader : ";
        if (nullptr == errorBlob.Get()) msg += fileName;
        else msg += StringHelper::StringToWideString((char*)errorBlob->GetBufferPointer());
        Log::ErrorMsg(hr, msg);
        return false;
    }

    //정점 쉐이더 생성
    hr = pDevice->CreateVertexShader(vertexBuffer.Get()->GetBufferPointer(), vertexBuffer.Get()->GetBufferSize(), NULL,
        vertexShader.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to create vertex shader : ";
        msg += fileName;
        Log::ErrorMsg(hr, msg);
        return false;
    }
        
    //Input Layout 생성
    hr = pDevice->CreateInputLayout(layoutDesc, numElements, vertexBuffer->GetBufferPointer(), vertexBuffer->GetBufferSize(), inputLayout.GetAddressOf());
    if (FAILED(hr))
    {
        Log::ErrorMsg(hr,"Failed to create input layout .");
        return false;
    }
    return true;
}//VertexShader::Initalize()

bool PixelShader::Initialize(ID3D11Device* pDevice, std::wstring fileName)
{
    Microsoft::WRL::ComPtr<ID3D10Blob>errorBlob, pixelBuffer;

    //픽셀 쉐이더 컴파일
    //ps_5_0(pixel shader 5.0) DirectX 11에서는 5.0버전을 사용
    HRESULT hr = D3DCompileFromFile(fileName.c_str(), NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        pixelBuffer.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to compile Shader : ";
        if (nullptr == errorBlob.Get()) msg += fileName;
        else msg += StringHelper::StringToWideString((char*)errorBlob->GetBufferPointer());
        Log::ErrorMsg(hr, msg);
        return false;
    }
    //픽셀 쉐이더 생성
    hr = pDevice->CreatePixelShader(pixelBuffer.Get()->GetBufferPointer(), pixelBuffer.Get()->GetBufferSize(), NULL, pixelShader.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to create pixel shader :";
        msg += fileName;
        Log::ErrorMsg(hr, msg);
        return false;
    }
    return true;
}

//HLSL(High Level Shader Language)
//DirectX에서 사용되는 셰이더 언어
//메모장 등을 이용하여 *.hlsl을 생성하여 프로젝트 폴더에 포함(프로젝트에는 추가할 필요 x)
//(파일확장자는 어떤것을 해도 상관없어, 개발자들이 구분하기 위해 적는것)
//정점셰이더의 Output 형식 = 픽셀 세이더의 Input형식은 반드시 같아야한다.
//정점 / 픽셀 셰이더(Vertex / Pixel Shader)의 함수명이 main이 아니어도 상관 없지만, 메인 함수의 이름은
//반드시 컴파일에 입력핚 셰이더 함수명과 같아야 핚다
//SV(System value,의미체계) : https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
//의도한 매개변수 사용에 대한 정보를 전달하는 셰이더 입력 또는 출력에 연결된 문자열,
//셰이더 단계 간에 전달되는 모든 변수에 SV 필요


//프로젝트 폴더 내에 .hlsl 파일 확장자에 함수 추가해놨음
//color.ps.hls , color.vs.hlsl (프로젝트 파일에 추가)



