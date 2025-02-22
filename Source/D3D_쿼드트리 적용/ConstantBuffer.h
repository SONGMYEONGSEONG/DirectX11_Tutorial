//ConstantBuffer.h
//상수 구조체를 Vertex Shader에 알려줄 상수 버퍼
// 정점 및 픽셀 셰이더에서 사용될 상수를 모아 놓은 버퍼이다.
#pragma once
#include "Log.h"
#include "Constant.h"
#include <wrl/client.h>
#include <d3d11.h>
template<typename T>
class ConstantBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext* deviceContext;
	ConstantBuffer(const ConstantBuffer& rhs);

public:
	ConstantBuffer() : buffer(nullptr), deviceContext(nullptr) {}

	// 사용 편의를 위해 public 변수로 선언
	T data;

	HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		this->deviceContext = deviceContext;

		if (nullptr != buffer.Get()) buffer.Reset();

		D3D11_BUFFER_DESC desc = { 0 };
		// 쉐이더의 cbuffer(constant buffer)는 16byte단위로 정렬.
		//외부에서 Init을 할떄 : <자료형(T)>Initialize(매개변수들) //자료형을들고와서 사이즈(16byte)에 맞게 정렬한다.
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		//매 프레임마다 업데이트 되기 때문에 동적(dynamic)으로 설정.
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		// CPU는 쓰기 (write)만 하기 때문에 사용에 맞춘다.
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

		return device->CreateBuffer(&desc, NULL, buffer.GetAddressOf());
	}


	//Map : 메모리(=버퍼) 락 <> Unmap : 메모릭(=버퍼) 락 해제
	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		HRESULT hr = deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if (FAILED(hr))
		{
			Log::ErrorMsg(hr, "Failed to map (memory lock) constant buffer.");
			return false;
		}

		CopyMemory(resource.pData, &data, sizeof(T));
		deviceContext->Unmap(buffer.Get(), 0);

		return true;
	}


	ID3D11Buffer* Get() const
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}
};

