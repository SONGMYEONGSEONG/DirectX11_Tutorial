#include "Texture.h"
Texture::Texture(ID3D11Device* device, const Color& color)
{
	InitializeColorTexture(device, color);
}

Texture::Texture(ID3D11Device* device, const std::string& filename)
{
	DirectX::ScratchImage image;
	if (LoadFromTextureFile(filename, image))
	{
		//Texture2D 버프를 만들지 않는 이유,
		//1.현재 사용하고있는데 DirexTEX.lib를 사용하고 있기 떄문에 이미지 로드 후 데이터만 바로 보내주면 버퍼가 필요없기떄문
		//(이 lib 말고 다른 lib같은 경우 이미지 로드해도 buffer가 필요해서 만들수도 있음, 예제마다 lib가 다른경우)
		//2. Texture2D 버퍼에서 필요한 모든 정보를 image 변수가 가지고 있기떄문에 버퍼를 안거치고 인자값으로 넣어서 적용시킨것(textureView = 쉐이더리소스뷰 자료형)
		HRESULT hr = DirectX::CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), textureView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view.");
		return;
	}
	InitializeColorTexture(device, Colors::UnhandledTextureColor); // 텍스쳐 로드에 실패할경우 color 구조체의 색을 가져옴 
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return textureView.GetAddressOf();
}

bool Texture::LoadFromTextureFile(const std::string& filename, DirectX::ScratchImage& scratchImage)
{
	HRESULT hr = E_FAIL;
	std::string extention = StringHelper::GetFileExtention(filename);
	std::wstring w_filename = StringHelper::StringToWideString(filename);

	if ("dds" == extention) hr = DirectX::LoadFromDDSFile(w_filename.c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, nullptr, scratchImage);
	//dds는 D3D에 최적화 된 포멧형식이기때문에 실질적으로는 잘 사용하지 않는다.
	//그러기 떄문에 tga포멧형식을 사용하는 DirexTEX.lib를 요번 예제에서 사용한것 (실질적으로는 jpen,png를 더 자주 쓰며 가끔 tga를 더 쓰기 때문)
	else if ("tga" == extention) hr = DirectX::LoadFromTGAFile(w_filename.c_str(), nullptr, scratchImage);
	else hr = DirectX::LoadFromWICFile(w_filename.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, nullptr, scratchImage);
	
	if (FAILED(hr)) return false;
	return true;

}

void Texture::InitializeColorTexture(ID3D11Device* device, const Color& color)
{

	CD3D11_TEXTURE2D_DESC texDesc(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, 1u, 1u);
	ID3D11Texture2D* p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &color;
	data.SysMemPitch = sizeof(Color);
	HRESULT hr = device->CreateTexture2D(&texDesc, &data, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to Initialize texture from color data.");

	texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D, texDesc.Format);
	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create resource view from texture generated from color data.");

}

