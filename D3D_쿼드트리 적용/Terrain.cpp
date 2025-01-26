#include "Terrain.h"
bool Terrain::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer, const std::string& filename, const std::string& texName,
	int numRepeat)
{
	//Terrain ��������� ���ڰ� ���� 
	this->device = device;
	this->deviceContext = deviceContext;
	this->vsConstantBuffer = &vsConstantBuffer;

	if (1 > numRepeat) numRepeat = 1;
	if (!LoadHeightMap(filename, numRepeat)) return false;
	if (PermitQuadTree(width) && PermitQuadTree(height)) // w,h ���� 2^n+1�� �´��� üũ//w,h�� LoadHeightMap�Լ����� heightmap�� width,height�� ������ 
	{
		quadTree = std::make_unique<QuadTree>();
		//data() : �����̳��� ù���� ���Ҹ� �����ͷ� ��ȯ�ϴ� �Լ� 
		quadTree->Initialize(vertices.data(), width, height);
	}

	//data() �Լ��� �����̳��� ù ��° ���Ҹ� ����Ű�� �����͸� ��ȯ
	HRESULT hr = vertexBuffer.Initialize(device, vertices.data(), vertices.size()); // vertexbuffer�� vertex data�� init
	COM_ERROR_IF_FAILED(hr, "Failed to Initialize vertex buffer for terrain.");

	hr = indexBuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to Initialize index buffer for terrain.");

	//Texture �����ڿ��� �ؽ����̸��� ���ڰ����� ������ �ؽ��� �̹����� �ε��� 
	textures.push_back(Texture(device, texName));

	return true;
}

void Terrain::Draw(const XMMATRIX& viewProjectionMatrix, Frustum* frustum)
{
	if (frustum && quadTree) // ����ü ��ü�� �ְ� ����Ʈ�� �� �����ϴ� ��� ���� 
	{
		quadTree->ProcessFrustumCulling(frustum); // ����ü ��ü�� ���ڰ����� �������� Ŭ���� ���θ� üũ
		indices = quadTree->GenerateIndices(); // Ŭ���� ���ο� ���� �ε��� ���͸� ����

		indexBuffer.Initialize(device, indices.data(), indices.size()); // ������ �ε��� ���͸� �ε��� ���ۿ� init (�׸������� init�� �Ѵ�)
	}

	deviceContext->VSSetConstantBuffers(0, 1, vsConstantBuffer->GetAddressOf());
	vsConstantBuffer->data.world = XMMatrixIdentity();
	vsConstantBuffer->data.viewProjection = viewProjectionMatrix;
	vsConstantBuffer->ApplyChanges();

	deviceContext->PSSetShaderResources(0, 1, textures[0].GetTextureResourceViewAddress());

	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indexBuffer.IndexCount(), 0u, 0);
}

bool Terrain::LoadHeightMap(const std::string& filename, const int& numRepeat)
{
	FILE* filePtr;
	// rb : read binary (���̳ʸ� �������� ����), �����ϸ� 0�� ��ȯ
	if (0 != fopen_s(&filePtr, filename.c_str(), "rb")) return false;

	const size_t count = 1;
	BITMAPFILEHEADER bitmapFileHeader;
	// ��ȯ ���� count���� ������ ���� �Ǵ� ���� ��.
	if (count != fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), count, filePtr)) return false;

	const size_t size = bitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER);
	LPBYTE bitmapImage = new BYTE[size];
	if (count != fread(bitmapImage, size, count, filePtr)) return false;

	//reinterpret_cast : ���� ��ȯ ������, ������ ���İ� ��ȯ, �����Ϳ� ������ ��ȯ , ���� ���� �� ��ȯ�� ��� 
	LPBITMAPINFOHEADER lpBIH = reinterpret_cast<LPBITMAPINFOHEADER>(bitmapImage);
	width = lpBIH->biWidth; //x��
	height = lpBIH->biHeight; //z��

	const int byteCount = (lpBIH->biBitCount / 8);
	const size_t pixelWidth = (width * byteCount);
	const int value = (pixelWidth % 4);
	const int align4byte = (value ? (pixelWidth + 4 - value) : pixelWidth); // row pixel ũ�⸦ 4byte�� �����.
	const int cols = height - 1;
	const size_t PAL_size = (24 == lpBIH->biBitCount ? 0 : (sizeof(RGBQUAD) * (1 << lpBIH->biBitCount))); // PAL : �÷� ���ڵ� ���.
	const int startIndex = sizeof(BITMAPINFOHEADER) + PAL_size;

	int index = 0;
	float y = 0, heightFactor = 1.0f / 10.0f;
	float invWidth = (1.0f / width) * numRepeat, invHeight = (1.0f / height) * numRepeat;
	for (int z = 0; height > z; z++)
	{
		for (int x = 0; width > x; x++)
		{
			index = x * byteCount + (cols - z) * align4byte;
			//������ �����ϰ� ������ �ʵ��� hegihtFactor�� ���̸� ���ҽ��� �ε巴�� ���̵��� �Ѵ�.
			y = (float)bitmapImage[startIndex + index] * heightFactor;

			ModelVertex v;
			v.pos.x = x;
			v.pos.y = y;
			v.pos.z = z;

			v.texCoord.x = x * invWidth;
			v.texCoord.y = z * invHeight;

			vertices.push_back(v);
		}
	}

	delete[] bitmapImage; // �Ҵ��ؼ� ��������� �Ҵ������� 
	bitmapImage = nullptr;

	const int rows = width - 1;
	DWORD index0, index1, index2, index3;
	for (DWORD z = 0; cols > z; z++)
	{
		for (DWORD x = 0; rows > x; x++)
		{
			index0 = x + z * width;		// bottom left
			index1 = index0 + 1;		// bottom right
			index2 = index0 + width;	//top left
			index3 = index1 + width;	//top right;

			indices.push_back(index0);
			indices.push_back(index2);
			indices.push_back(index1);

			indices.push_back(index1);
			indices.push_back(index2);
			indices.push_back(index3);

			// setup vertices normal (��������)
			VerticesNormalVectorSetup(index0, index2, index1);
		}
	}
	return true;
}//Terrain::LoadHeightMap()

void Terrain::VerticesNormalVectorSetup(const DWORD& bottomLeftIndex, const DWORD& topLeftIndex, const DWORD& bottomRightIndex)
{
	XMFLOAT3 pos1 = vertices[bottomLeftIndex].pos;
	XMFLOAT3 pos2 = vertices[topLeftIndex].pos;
	XMFLOAT3 pos3 = vertices[bottomRightIndex].pos;

	//�������� ���ϴ� �� - > ������ ����(cross_product) : �� ���Ϳ� �������� �Ǵ� ���͸� ã�� �� 
	XMVECTOR edge1 = XMVectorSet((pos2.x - pos1.x), (pos2.y - pos1.y), (pos2.z - pos1.z), 1.0f);
	XMVECTOR edge2 = XMVectorSet((pos3.x - pos2.x), (pos3.y - pos2.y), (pos3.z - pos2.z), 1.0f);
	XMVECTOR cross = XMVector3Normalize(XMVector3Cross(edge1, edge2)); // �������� ���ϰ� ����ȭ ��

	XMStoreFloat3(&vertices[bottomLeftIndex].normal, cross);//bottom,left vertex�� normal(��������)���� ����(cross)

}