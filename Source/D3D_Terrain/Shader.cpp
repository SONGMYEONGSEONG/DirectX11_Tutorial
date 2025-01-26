//Shader.cpp
#include "Shader.h"

//Mesh : �ٸ�ü�� ���¸� �����ϴ� �������� ���� 
//ID3D10Blob :
// 1.Mesh(�޽�)  ����ȭ �� �ε� �۾� �߿� vertex(����), ������ �� material(����) ������ �����ϴ� ������ ����
// 2.vertex,Geomerty Shader(���� �������̴�) �� VS(vertex Shader)�� �������ϴ� API���� ��ü �ڵ� �� ���� �޽����� ��ȯ
// Blob�� ��� ���̷�Ʈ 3D ������ �ڵ�� ��밡�� 

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

    //���� ���̴� ������
    //vs_5_0(vertex shader 5.0) DirectX 11������ 5.0������ ���
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

    //���� ���̴� ����
    hr = pDevice->CreateVertexShader(vertexBuffer.Get()->GetBufferPointer(), vertexBuffer.Get()->GetBufferSize(), NULL,
        vertexShader.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to create vertex shader : ";
        msg += fileName;
        Log::ErrorMsg(hr, msg);
        return false;
    }
        
    //Input Layout ����
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

    //�ȼ� ���̴� ������
    //ps_5_0(pixel shader 5.0) DirectX 11������ 5.0������ ���
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
    //�ȼ� ���̴� ����
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
//DirectX���� ���Ǵ� ���̴� ���
//�޸��� ���� �̿��Ͽ� *.hlsl�� �����Ͽ� ������Ʈ ������ ����(������Ʈ���� �߰��� �ʿ� x)
//(����Ȯ���ڴ� ����� �ص� �������, �����ڵ��� �����ϱ� ���� ���°�)
//�������̴��� Output ���� = �ȼ� ���̴��� Input������ �ݵ�� ���ƾ��Ѵ�.
//���� / �ȼ� ���̴�(Vertex / Pixel Shader)�� �Լ����� main�� �ƴϾ ��� ������, ���� �Լ��� �̸���
//�ݵ�� �����Ͽ� �Է��� ���̴� �Լ���� ���ƾ� ����
//SV(System value,�ǹ�ü��) : https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
//�ǵ��� �Ű����� ��뿡 ���� ������ �����ϴ� ���̴� �Է� �Ǵ� ��¿� ����� ���ڿ�,
//���̴� �ܰ� ���� ���޵Ǵ� ��� ������ SV �ʿ�


//������Ʈ ���� ���� .hlsl ���� Ȯ���ڿ� �Լ� �߰��س���
//color.ps.hls , color.vs.hlsl (������Ʈ ���Ͽ� �߰�)



