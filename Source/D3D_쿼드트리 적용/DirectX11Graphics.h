//DirectX 11 Graphic ��ü ����
//DriectX 11 ������� Scene�� �������ϱ� ���� �׷��Ƚ� ��ü
//Comptr(Componete object model Pointer)�� �̿��Ͽ� COM ��ü�� �ڿ�(�޸�)������ �����ϰ� �Ҵ� �� ������ �� �� �ִ�.
//(COM�� ����Ʈ ������)

#pragma once
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"d3d11.lib")
#include <wrl/client.h> // Microsoft::WRL::ComPtr�� ����ϱ� ���� �߰�
#include <d3d11.h>
#include "WindowContainer.h"
#include "Shader.h" //VertexBuffer.h �ۼ��� �߰�
//#include "VertexBuffer.h" //VertexBuffer.h �ۼ��� �߰� // Mesh�߰��� �ڵ� ���� 
//#include "IndexBuffer.h" //IndexBuffer.h �ۼ��� �߰�  // Mesh�߰��� �ڵ� ���� 
//#include "ConstantBuffer.h"//z/stencil buffer ����  // Mesh�߰��� �ڵ� ���� 
#include "DXGameObject.h"//z/stencil buffer ����
#include "Components/DXCamera.h" //z/stencil buffer ����
//#include "Texture.h" //Texture Ŭ���� �ۼ��� �߰�  // Mesh�߰��� �ڵ� ���� 
#include "Components/DXMeshRenderer.h" // Mesh�߰��� �ڵ� �߰� 
#include "PrimitiveModel.h"// Mesh�߰��� �ڵ� �߰� 
#include "Frustum.h"//����ü �ø� �۾� �߰� 
#include "Components/DXSpriteRenderer.h"//sprite2D �۾� �߰�
#include "Terrain.h"//Terrain(����) �߰�
class DirectX11Graphics final : public Graphics
{
private:
	Timer* timer = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain; // ����ڿ��� �������� ǥ���ϴµ� ���Ǵ� ������ �÷��� , ����۸� �������ԵǴµ� ��� 
	Microsoft::WRL::ComPtr<ID3D11Device> device; 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext; // ���÷��� �Ǵ� �����Ϳ� ���� ����̽��� �׸��� Ư���� ���� ������ �����ϴ� Windows ������ ����
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView; //ID3D11View �������̽��� �ڽ�Ŭ���� �� �ϳ� , ����ۿ� ����Ǵµ� ���  
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView; //z/stencil buffer ����
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer; //z/stencil buffer ����
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState; //z/stencil buffer ����


	//VertexBuffer<VertexColor> vertexBuffer;//VertexBuffer.h �ۼ��� �߰�
	//IndexBuffer indexBuffer; //IndexBuffer.h �ۼ��� �߰�
	VertexShader vertexShader;//VertexBuffer.h �ۼ��� �߰�
	PixelShader pixelShader;//VertexBuffer.h �ۼ��� �߰�
	ConstantBuffer<Matrices> constantMatricesBuffer;//z/stencil buffer ����
	DXGameObject cameraObj;//z/stencil buffer ����
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState; //Texture Ŭ���� �ۼ��� �߰� 
	//std::unique_ptr<Texture> texture; //Texture Ŭ���� �ۼ��� �߰� 
	//VertexBuffer<VertexTexture> texVertexBuffer; //Texture Ŭ���� �ۼ��� �߰� 
	//VertexBuffer<ModelVertex> modelVertexBuffer;//���� �߰��ϸ鼭 �۾�
	ConstantBuffer<Light> constantLightBuffer;//���� �߰��ϸ鼭 �۾�

	std::unique_ptr <PrimitiveModel> Primitive;
	DXGameObject PrimitiveObj;

	Frustum frustum;//����ü �ø� �۾� �߰� 
	/*sprite2D*/
	VertexShader sprite_vertexShader;
	PixelShader sprite_pixelShader;
	DXGameObject camera2D;
	DXGameObject sprite;
	/*========*/

	/*Terrain*/
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateWireframe; // �����Ͷ������� state���� �����ִ� ������ 
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

	bool InitializeShader();//VertexBuffer.h �ۼ��� �߰�
	bool InitializeScene();//VertexBuffer.h �ۼ��� �߰�
};

