//IndexBuffer.h
//Mesh�� �׸��� �ѹ� ���� VertexBuffer�� �ٽ� ������ �ʴ´�.(�߿�)
//���ϴ� Mesh�� �׸��� ���ؼ��� �ߺ��Ǵ� VertexBuffer�� ������ �ϱ⿡ �޸� ���� �߻�
//DirectX 8���� IndexBuffer�� �߰���
//vertex �迭�� �ε����� ���� vertex�� �����Ͽ� Mesh�� �׸���.
//->�ѹ� ���� VertexBuffer�� IndexBuffer�� �����Ͽ� �����Ѵ�.

#pragma once
#include<wrl/client.h> // Microsoft::WRL::ComPtr�� ����ϱ� ���� �߰�
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
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount; // ���� ũ��
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

