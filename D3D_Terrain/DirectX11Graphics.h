//DirectX 11 Graphic 객체 생성
//DriectX 11 기반으로 Scene을 렌더링하기 위한 그래픽스 객체
//Comptr(Componete object model Pointer)을 이용하여 COM 객체의 자원(메모리)관리를 안전하게 할당 및 해제를 할 수 있다.
//(COM용 스마트 포인터)

#pragma once
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"d3d11.lib")
#include <wrl/client.h> // Microsoft::WRL::ComPtr를 사용하기 위해 추가
#include <d3d11.h>
#include "WindowContainer.h"
#include "Shader.h" //VertexBuffer.h 작성후 추가
//#include "VertexBuffer.h" //VertexBuffer.h 작성후 추가 // Mesh추가후 코드 정리 
//#include "IndexBuffer.h" //IndexBuffer.h 작성후 추가  // Mesh추가후 코드 정리 
//#include "ConstantBuffer.h"//z/stencil buffer 구현  // Mesh추가후 코드 정리 
#include "DXGameObject.h"//z/stencil buffer 구현
#include "Components/DXCamera.h" //z/stencil buffer 구현
//#include "Texture.h" //Texture 클래스 작성후 추가  // Mesh추가후 코드 정리 
#include "Components/DXMeshRenderer.h" // Mesh추가후 코드 추가 
#include "PrimitiveModel.h"// Mesh추가후 코드 추가 
#include "Frustum.h"//절두체 컬링 작업 추가 
#include "Components/DXSpriteRenderer.h"//sprite2D 작업 추가
#include "Terrain.h"//Terrain(지형) 추가
class DirectX11Graphics final : public Graphics
{
private:
	Timer* timer = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain; // 사용자에게 프레임을 표시하는데 사용되는 버퍼의 컬렉션 , 백버퍼를 가져오게되는데 사용 
	Microsoft::WRL::ComPtr<ID3D11Device> device; 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext; // 디스플레이 또는 프린터와 같은 디바이스의 그리기 특성에 대한 정보를 포함하는 Windows 데이터 구조
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView; //ID3D11View 인터페이스의 자식클래스 중 하나 , 백버퍼와 연결되는데 사용  
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView; //z/stencil buffer 구현
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer; //z/stencil buffer 구현
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState; //z/stencil buffer 구현


	//VertexBuffer<VertexColor> vertexBuffer;//VertexBuffer.h 작성후 추가
	//IndexBuffer indexBuffer; //IndexBuffer.h 작성후 추가
	VertexShader vertexShader;//VertexBuffer.h 작성후 추가
	PixelShader pixelShader;//VertexBuffer.h 작성후 추가
	ConstantBuffer<Matrices> constantMatricesBuffer;//z/stencil buffer 구현
	DXGameObject cameraObj;//z/stencil buffer 구현
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState; //Texture 클래스 작성후 추가 
	//std::unique_ptr<Texture> texture; //Texture 클래스 작성후 추가 
	//VertexBuffer<VertexTexture> texVertexBuffer; //Texture 클래스 작성후 추가 
	//VertexBuffer<ModelVertex> modelVertexBuffer;//조명 추가하면서 작업
	ConstantBuffer<Light> constantLightBuffer;//조명 추가하면서 작업

	std::unique_ptr <PrimitiveModel> Primitive;
	DXGameObject PrimitiveObj;

	Frustum frustum;//절두체 컬링 작업 추가 
	/*sprite2D*/
	VertexShader sprite_vertexShader;
	PixelShader sprite_pixelShader;
	DXGameObject camera2D;
	DXGameObject sprite;
	/*========*/

	/*Terrain*/
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateWireframe; // 레스터라이즈의 state값을 갖고있는 포인터 
	Terrain terrain;
	/*=======*/

	/*TEST*/
	float light_dir_x = 1.0f, light_dir_y = -1.0f, light_dir_z = 1.0f;
public:
	virtual bool Initialize(WindowContainer* pWindowContainer) override;
	virtual void Update() override;
	virtual void RenderFrame() override;

private:
	bool InitializeDirectX(HWND hWnd, bool fullScreen);

	bool InitializeShader();//VertexBuffer.h 작성후 추가
	bool InitializeScene();//VertexBuffer.h 작성후 추가
};

