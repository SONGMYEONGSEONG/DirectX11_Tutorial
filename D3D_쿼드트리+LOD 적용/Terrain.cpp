#include "Terrain.h"
bool Terrain::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<Matrices>& vsConstantBuffer, const std::string& filename, const std::string& texName,
	int numRepeat)
{
	//Terrain 멤버변수에 인자값 정의 
	this->device = device;
	this->deviceContext = deviceContext;
	this->vsConstantBuffer = &vsConstantBuffer;

	if (1 > numRepeat) numRepeat = 1;
	if (!LoadHeightMap(filename, numRepeat)) return false;
	if (PermitQuadTree(width) && PermitQuadTree(height)) // w,h 값이 2^n+1이 맞는지 체크//w,h는 LoadHeightMap함수에서 heightmap의 width,height를 가져옴 
	{
		quadTree = std::make_unique<QuadTree>();
		//data() : 컨테이너의 첫번쨰 원소를 포인터로 반환하는 함수 
		quadTree->Initialize(vertices.data(), width, height);
	}

	//data() 함수는 컨테이너의 첫 번째 원소를 가리키는 포인터를 반환
	HRESULT hr = vertexBuffer.Initialize(device, vertices.data(), vertices.size()); // vertexbuffer에 vertex data값 init
	COM_ERROR_IF_FAILED(hr, "Failed to Initialize vertex buffer for terrain.");

	hr = indexBuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to Initialize index buffer for terrain.");

	//Texture 생성자에서 텍스쳐이름을 인자값으로 가져와 텍스쳐 이미지를 로드함 
	textures.push_back(Texture(device, texName));

	return true;
}

void Terrain::Draw(const XMMATRIX& viewProjectionMatrix, Frustum* frustum)
{
	if (frustum && quadTree) // 절두체 객체가 있고 쿼드트리 도 존재하는 경우 실행 
	{
		quadTree->ProcessFrustumCulling(frustum); // 절두체 객체를 인자값으로 가져가서 클리핑 여부를 체크
		indices = quadTree->GenerateIndices(); // 클리핑 여부에 따른 인덱스 벡터를 생성

		indexBuffer.Initialize(device, indices.data(), indices.size()); // 생성된 인덱스 벡터를 인덱스 버퍼에 init (그릴떄마다 init을 한다)
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
	// rb : read binary (바이너리 형식으로 읽음), 성공하면 0을 반환
	if (0 != fopen_s(&filePtr, filename.c_str(), "rb")) return false;

	const size_t count = 1;
	BITMAPFILEHEADER bitmapFileHeader;
	// 반환 값이 count보다 작으면 오류 또는 파일 끝.
	if (count != fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), count, filePtr)) return false;

	const size_t size = bitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER);
	LPBYTE bitmapImage = new BYTE[size];
	if (count != fread(bitmapImage, size, count, filePtr)) return false;

	//reinterpret_cast : 형식 변환 연산자, 포인터 형식간 변환, 포인터와 정수간 변환 , 참조 형식 간 변환에 사용 
	LPBITMAPINFOHEADER lpBIH = reinterpret_cast<LPBITMAPINFOHEADER>(bitmapImage);
	width = lpBIH->biWidth; //x축
	height = lpBIH->biHeight; //z축

	const int byteCount = (lpBIH->biBitCount / 8);
	const size_t pixelWidth = (width * byteCount);
	const int value = (pixelWidth % 4);
	const int align4byte = (value ? (pixelWidth + 4 - value) : pixelWidth); // row pixel 크기를 4byte로 맞춘다.
	const int cols = height - 1;
	const size_t PAL_size = (24 == lpBIH->biBitCount ? 0 : (sizeof(RGBQUAD) * (1 << lpBIH->biBitCount))); // PAL : 컬러 인코딩 방식.
	const int startIndex = sizeof(BITMAPINFOHEADER) + PAL_size;

	int index = 0;
	float y = 0, heightFactor = 1.0f / 10.0f;
	float invWidth = (1.0f / width) * numRepeat, invHeight = (1.0f / height) * numRepeat;
	for (int z = 0; height > z; z++)
	{
		for (int x = 0; width > x; x++)
		{
			index = x * byteCount + (cols - z) * align4byte;
			//지형이 뾰족하게 보이지 않도록 hegihtFactor로 높이를 감소시켜 부드럽게 보이도록 한다.
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

	delete[] bitmapImage; // 할당해서 사용했으니 할당해지함 
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

			// setup vertices normal (법선벡터)
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

	//법선벡터 구하는 것 - > 벡터의 외적(cross_product) : 두 벡터와 수직으로 되는 벡터를 찾는 것 
	XMVECTOR edge1 = XMVectorSet((pos2.x - pos1.x), (pos2.y - pos1.y), (pos2.z - pos1.z), 1.0f);
	XMVECTOR edge2 = XMVectorSet((pos3.x - pos2.x), (pos3.y - pos2.y), (pos3.z - pos2.z), 1.0f);
	XMVECTOR cross = XMVector3Normalize(XMVector3Cross(edge1, edge2)); // 법선벡터 구하고 정규화 함

	XMStoreFloat3(&vertices[bottomLeftIndex].normal, cross);//bottom,left vertex의 normal(법선벡터)값을 대입(cross)

}