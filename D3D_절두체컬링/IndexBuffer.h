//IndexBuffer.h
//Mesh를 그릴떄 한번 사용된 VertexBuffer는 다시 사용되지 않는다.(중요)
//원하는 Mesh를 그리기 위해서는 중복되는 VertexBuffer를 만들어야 하기에 메모리 낭비 발생
//DirectX 8부터 IndexBuffer가 추가됨
//vertex 배열의 인덱스를 사용된 vertex를 재사용하여 Mesh를 그린다.
//->한번 사용된 VertexBuffer를 IndexBuffer에 저장하여 재사용한다.

#pragma once
#include<wrl/client.h> // Microsoft::WRL::ComPtr를 사용하기 윟애 추가
#include <d3d11.h>
class IndexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>buffer;
	UINT indexCount;

public:
	IndexBuffer() : buffer(nullptr),indexCount(0) {}
	IndexBuffer(const IndexBuffer& rhs)
	{
		buffer = rhs.buffer;
		indexCount = rhs.indexCount;
	}
	IndexBuffer& operator=(const IndexBuffer& rhs)
	{
		buffer = rhs.buffer;
		indexCount = rhs.indexCount;
		return *this;
	}

	HRESULT Initialize(ID3D11Device* device, DWORD* indices, UINT indexCount)
	{
		if (nullptr != buffer.Get()) buffer.Reset();

		this->indexCount = indexCount;

		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount; // 버퍼 크기
		indexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = indices;

		return device->CreateBuffer(&indexBufferDesc, &indexData, buffer.GetAddressOf());
	}

	ID3D11Buffer* Get() const { return buffer.Get(); }
	ID3D11Buffer* const* GetAddressOf() const { return GetAddressOf(); }

	UINT IndexCount() const { return indexCount; }
};

