#include "DirectX11Graphics.h"

bool DirectX11Graphics::Initialize(WindowContainer* pWindowContainer)
{
	Window* pWindow = pWindowContainer->GetWindow();
	screenWidth = pWindow->GetScreenWidth();
	screenHeight = pWindow->GetScreenHeight();

	if (!InitializeDirectX(pWindow->GetHWND(), pWindow->IsFullScreen())) return false;

	timer = pWindowContainer->GetTimer();

	if (!InitializeShader()) return false;//VertexBuffer.h �ۼ��� �߰�
	if (!InitializeScene()) return false;//VertexBuffer.h �ۼ��� �߰�

	return true;
}

void DirectX11Graphics::Update()
{
	const float deltaTime = timer->GetMilisecondsElapesed();
	static float earthYaw = 0, SunYaw =0 , MoonYaw =0;

	earthYaw += deltaTime * 0.1f;
	SunYaw += deltaTime * 0.05f;
	MoonYaw += deltaTime * 0.1f;
	parentPrimitiveObj.GetTransform()->SetLocalRotation({ 0, SunYaw, 0 }); // �¾�
	Sun_Earth_node.GetTransform()->SetLocalRotation({ 0, SunYaw, 0 }); // ���� ����
	PrimitiveObj.GetTransform()->SetLocalRotation({ 0,earthYaw, 0 });  // ����
	Earth_Moon_node.GetTransform()->SetLocalRotation({ 0, MoonYaw, 0 }); // �� ����
	childPrimitiveObj.GetTransform()->SetLocalRotation({ 0, MoonYaw, 0 }); // ��

	/* z / stencil buffer ���� */
	static float accel = 1.0f; //���ӵ�
	//const float deltaTime = timer->GetMilisecondsElapesed();
	static int light_reverse_num = 1; //���� ���� ��ȣ��
	if (Input::GetKeyboard()->IsDown(VK_SHIFT))
	{
		accel = 5.0f; // Shift key Down, �̵� 5�� ����
	}
	if (Input::GetKeyboard()->IsUp(VK_SHIFT))
	{
		accel = 1.0f;
	}

	if (Input::GetKeyboard()->IsDown('1'))
	{
		light_reverse_num = -1;
	}
	if (Input::GetKeyboard()->IsUp('1'))
	{
		light_reverse_num = 1;
	}

	//ī�޶� WASD(�����¿�) �̵�
	if (Input::GetKeyboard()->IsPressed('D')) cameraObj.GetTransform()->AdjustPosition(cameraObj.GetTransform()->GetRight() * deltaTime * 0.01f * accel);
	if (Input::GetKeyboard()->IsPressed('A')) cameraObj.GetTransform()->AdjustPosition(-cameraObj.GetTransform()->GetRight() * deltaTime * 0.01f * accel);
	if (Input::GetKeyboard()->IsPressed('W')) cameraObj.GetTransform()->AdjustPosition(cameraObj.GetTransform()->GetUp() * deltaTime * 0.01f * accel);
	if (Input::GetKeyboard()->IsPressed('S')) cameraObj.GetTransform()->AdjustPosition(-cameraObj.GetTransform()->GetUp() * deltaTime * 0.01f * accel);
	

	//TEST - ���� ���� ����
	// �޼���ǥ�����
	//���� ���� =/= ���� , ���� ����(��ġ x, ����(����) O) 
	if (Input::GetKeyboard()->IsPressed('Q'))
	{
		//���� ������ ���������� �̵�(ȸ��), spceular�� ���ݴ�� �̵��ϴ°�ó�� ���ϰ�
		//������
		light_dir_x += deltaTime * 0.01f * light_reverse_num;
	}
	if (Input::GetKeyboard()->IsPressed('E'))
	{
		light_dir_y += deltaTime * 0.01f * light_reverse_num;
	}
	if (Input::GetKeyboard()->IsPressed('R'))
	{
		light_dir_z += deltaTime * 0.01f * light_reverse_num;
	}
	//light_dir_reset
	if (Input::GetKeyboard()->IsPressed('T'))
	{
		light_dir_x = light_dir_z = 1.0f;
		light_dir_y = -1.0f;
	}

	/**/
	MouseEvent mouseEvent = Input::GetMouse()->ReadEvent();
	switch (mouseEvent.eventType)
	{
		case MouseEventType::WheelVertical: //ī�޶� ��(Wheel up) / �� (Wheel down) �̵�.
		{
			cameraObj.GetTransform()->AdjustPosition(cameraObj.GetTransform()->GetForward() * deltaTime * mouseEvent.y * accel);
			break;
		}
		case MouseEventType::RawMove:
		{
			if (Input::GetMouse()->IsDown(ButtonType::Right))//ī�޶� x,y�� ȸ��(���콺 ������ Ŭ�� & ���콺 �̵�)
			{
				XMVECTOR v = XMVectorSet(mouseEvent.y, mouseEvent.x, 0.0f, 0.0f) * deltaTime * 0.1f;
				cameraObj.GetTransform()->AdjustRotation(v);
			}
			break;
		}

	}
	/**/
}

//Mesh �߰��� �ڵ� ���� 
void DirectX11Graphics::RenderFrame() // winapi�� ������۸��� ����(back���ۿ� �׸��Ÿ� ���� ȭ�鿡 �ű�� ����) 
{

	float color[4] = { 0.0f, 0.0f ,1.0f ,1.0f }; // {R,G,B,����}
	//�� ����(back surface)�� �����.
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	//z / Stencilbuffer�� �����.
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);

	//TODO CODE
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);//z,���ٽ� ���� ���� OM(��� ���ձ�)�� ����
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf()); //PS�� ������ ���÷� ����(address code)

	/* ���� �߰� */
	//material�� �����ִ� ���� �ݻ�� (mesh�� �����ִ� �ؽ��� �� , material ������ �����ִ� ����data) 
	constantLightBuffer.data.material.ambient = { 1.0f, 1.0f, 1.0f, 1.0f }; // ȯ�汤(���)
	constantLightBuffer.data.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���ݻ籤(���)
	constantLightBuffer.data.material.specular = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���ݻ籤(���)
	constantLightBuffer.data.material.specularPower = 50.0f; //���ݻ籤�� ��ī�ο�(����)

	//���� �������� �ʴ� ���� ��� ������ ĥ����, ��ü�� ���� �� �� ���� ������ ���� ȯ�汤�� ���� (0,0,0)�� ������� �ʴ´�.
	//light�� �����ִ� �ݻ�� 
	constantLightBuffer.data.globalAmbient = { 0.1f, 0.1f, 0.1f, 1.0f };// ���� ȯ�汤(������)
	constantLightBuffer.data.light_color = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���� ��(���)
	//����ȭ�� ����ص� ũ��� 1�̱� ������ �ᱹ ���� �ӵ�(����)�� �پ���.
	// 1 -> 100 - 1000 -> .... ���� Ŀ������ ����ȭ�� ũ��� 1�� �����ϱ⋚���� 
	XMStoreFloat4(&constantLightBuffer.data.light_direction, XMVector3Normalize(XMVectorSet(light_dir_x, light_dir_y, light_dir_z, 0.0f)));//���� ����
	XMStoreFloat4(&constantLightBuffer.data.camPosition, cameraObj.GetTransform()->GetPosition()); // ī�޶��� ��ġ .
	if (constantLightBuffer.ApplyChanges()) deviceContext->PSSetConstantBuffers(0, 1, constantLightBuffer.GetAddressOf());
	/**/

	//���̴� ����.
	deviceContext->IASetInputLayout(vertexShader.GetInputLayOut());
	deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);

	frustum.ConstructFrustum();//����ü �ø� �۾� �߰� 
	//GameObject Primitive Mesh Draw
	if (frustum.IsInFrustumBoundsCube(parentPrimitiveObj.GetTransform()->GetPosition(), XMVectorGetX(parentPrimitiveObj.GetTransform()->GetScale())/*bounds*/))//����ü �ø� �۾� �߰� 
		parentPrimitiveObj.Draw(cameraObj.GetComponent<DXCamera>()->GetViewProjectionMatrix());

	if (frustum.IsInFrustumBoundsCube(PrimitiveObj.GetTransform()->GetPosition(), XMVectorGetX(PrimitiveObj.GetTransform()->GetScale())/*bounds*/))//����ü �ø� �۾� �߰� 
		PrimitiveObj.Draw(cameraObj.GetComponent<DXCamera>()->GetViewProjectionMatrix());

	if (frustum.IsInFrustumBoundsCube(childPrimitiveObj.GetTransform()->GetPosition(), XMVectorGetX(childPrimitiveObj.GetTransform()->GetScale())/*bounds*/))//����ü �ø� �۾� �߰� 
		childPrimitiveObj.Draw(cameraObj.GetComponent<DXCamera>()->GetViewProjectionMatrix());

	swapChain->Present(NULL, NULL);
}

bool DirectX11Graphics::InitializeDirectX(HWND hWnd, bool fullScreen)
{
	try
	{
		// Swap Chain ������ ���� Description ����ü ����
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = screenWidth; // �� ������ ũ�� ����
		scd.BufferDesc.Height = screenHeight;
		scd.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

		// (0 / 1) : 1�ʿ� �ִ��� ���� �׸��� �Ѵ�.(v_sync ���ó���)
		//0/1�� ����͸��� �ֻ����� ��������ʰ� ȭ�鰻���� �϶�� ���̰� ���� �̰��� �ٲ㵵 ��������ȭ�� ������� �ʴ´�
		//��������ȭ�� ����Ϸ��� swapchain�� present�Լ����� Ȱ��ȭ�ؾߵǸ� , ����Ϳ� Hz�� �� ���������͸� ����Ͽ� ��������ȭ ó���� �Ѵ�. 
		scd.BufferDesc.RefreshRate.Numerator = 0;//���� 
		scd.BufferDesc.RefreshRate.Denominator = 1;//�и�(���� 0�� �Ǹ� �ȵ�)

		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // ���� �ֻ� �� ��� �ֻ� (�������� ����)
		//��� �ֻ� : ¦��,Ȧ���� �ֻ缱�� �����ư��� ����ϴ� ��� ex) 480i,720i...
		//���� �ֻ�: �ֻ缱�� 1�ʿ� 60���� �Ѳ����� ����ϴ� ���( Hz) ex) 480p,720p...
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;//�̹����� ����Ϳ� �°� Ȯ��/��� �ϴ� ���(�������� ����) 
	
		//MSAA(Multi Sample Anti Aliasing)�� ������� ����
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //����� ���ó(�뵵) ����.
		scd.BufferCount = 1; // �� ���۸� 1�� ���.

		scd.OutputWindow = hWnd; // �������� �̷���� ������(�� �ڵ�)�� ����.
		scd.Windowed = !fullScreen;

		scd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; // �� ���� ������ ���(present, ȭ�鿡 ���)�� ������ ������(discard)
		scd.Flags = 0;

		//feature level : ��� ������ DirectX�� ����� ���ΰ�.
		//����� ���� ��ǻ�͸� �����ϱ� ���Ͽ� ���� ������ DirectX ������ �߰�.
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
		};
		UINT featureLevelCount = ARRAYSIZE(featureLevels);

		// ========== DirectX11 Device, Device Context, Swap Chain ���� ==========
		// DirectX11 Device : ��ü�� �Ҵ� �� �����ϸ� �⺻ ������ �������ϸ� �׷��� ����̹� �� �ϵ����� ���

		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
			NULL, // D3D_DRIVER_TYPE_SOFTWARE�� �ƴ϶�� �ݵ�� NULL �̾�� �ϰ� �ƴϸ� NULL�̸� �ȵȴ�.
			NULL, // ��Ÿ�� ������ ���� flags
			featureLevels,
			featureLevelCount,
			D3D11_SDK_VERSION,
			&scd,
			swapChain.GetAddressOf(),
			device.GetAddressOf(),
			NULL,//�����Ǵ� ��� ����(feature level) output
			deviceContext.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create device and swap chain.");
		// ===================================================================

		// =========================== �� ���� ���� ==============================
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		//SwapChain�� �̿��Ͽ� �� ���۸� �����´�.
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to GetBuffer.");
		//�� ���۸� RenderTargetView�� ����.
		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");
		// =======================================================================

		// ============================ ����Ʈ ���� ==============================
		// ����Ʈ ������ �������� ���� ���, �� �۾��� ���� ����.
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight));
		deviceContext->RSSetViewports(1, &viewport); //NumViewports(1) : �� ��Ʈ�� ��.( ����Ʈ �迭�� ������������ �����Ͷ����� �ܰ迡 ���ε�)
		// =======================================================================

		// ========================= �����Ͷ����� ���� ===========================
		// Rasterizer State�� ���� �������� �ʾƵ� �⺻������ �����Ǿ� �ִ�. �ٸ� ���� ������ ����.
		CD3D11_RASTERIZER_DESC rasterDesc(D3D11_DEFAULT);
		// rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; ���� ������ ä���.
		// rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK; �ĸ� �ø�.
		// rasterDesc.FrontCounterClockwise = false; �ݽð����(Counter Clock wise) �׸��⸦ �޸�����.
		hr = device->CreateRasterizerState(&rasterDesc, rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
		// ======================================================================

		/* z / stencil buffer ���� */
		// ========================= ����/���Ľ� State ���� ===========================
		//���� ���� ���O, ���ٽ� ���� ���X
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
		// ======================================================================

		// ========================= ����/���Ľ� buffer ���� ===========================
		//Mip Level : �Ӹ�, ������ �׸��� �پ��� ������� ����� ��, �������� �ӵ��� ���̰� �ٸ����(�������)�� �ٿ��ش�.
		CD3D11_TEXTURE2D_DESC depthBufferDesc(DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT, screenWidth, screenHeight);
		depthBufferDesc.MipLevels = 1;
		//����/���ٽ� ���۷� ����� �� �̱� ������ depth/Stencil�� bind
		depthBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		hr = device->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to Create depth stencil view.");
		// ======================================================================
		/**/

		//Texture Ŭ���� �ۼ��� �߰�
		// ========================= ���÷� State ���� ===========================
		//Texture address mode �� �˰������� (�⺻���� �⺻)
		//��带 �����Ҷ� u,v,w(x,y,z)�� �� �Ȱ��� �������ָ� �ȴ�.
		//�޽��� �ؽ��ĸ� ������(���� ĥ�ϴ�) �����̶� �����Ͷ����� �ܰ迡�� ����ȴ�.(�����Ͷ����� ������ �ۼ��ϸ��)
		CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
		// ======================================================================

		// NumViews(1) : ���� ü���� ���� �� �� ���۸� 1�� ������� �Ͽ��� ������,
		//				 ���� Ÿ�� �� �ϳ��� ���.
		//NULL -> depthStencilView.Get() �� ���� ( z / stencil buffer ����)
		deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get()); // OM -> OutputMerger : Z-buufer,stencil-buffer �� ������� �ϴ°�
		deviceContext->RSSetState(rasterizerState.Get()); // RS ->Resterize�� ����

		//�⺻ ���� �������� ����Ͽ� �⺻ ������ ��� �׸������� ���� (�ﰢ������ ����)
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//vertexBuffer.h �ۼ��� �߰�

	}//try
	catch (COMException& exception)
	{
		Log::ErrorMsg(exception);
		return false;
	}
	return true;
}

bool DirectX11Graphics::InitializeShader()//VertexBuffer.h �ۼ��� �߰�
{
	//Shader? : 3D��ǻ�� �׷��ȿ��� ���������� ȭ�鿡 ����ϴ� �ȼ��� ���� �����ִ� �Լ� 
	D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0}, // vertexShader���� vertex ������ ������ ��ǥ�� ����(������ȯ,��ĵ��)
		//�ؽ��� ������ ����: �ؽ��� �޸� ���� �߸���, A32�� ���������(texture.hlsl�� flolat4�� ����ϱ� ������) 
		{ "COLOR",0,DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0}, // �����Ͷ��̵� �ܰ迡�� ������ �����͸� PixelShader���� �� �ȼ����� �����͸� ����
		{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// �ؽ��� 2d�̹��ڰ� �߰��Ǹ鼭 �޽��� ���� �ؽ��� ���̴��� �ʿ��ؼ� ����
		{"NORMAL",0,DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0},
		//������ ����� �ؽ��� �۾����ϸ鼭 ���� ���̴��� �ʿ��� -> ����(�븻���� �ʿ�) �븻���̴� 
	};
	UINT numElements = ARRAYSIZE(inputLayout);
	if (!vertexShader.Initialize(device.Get(), L"light.vs.hlsl", inputLayout, numElements)) return false;
	if (!pixelShader.Initialize(device.Get(), L"light.ps.hlsl")) return false;
	return true;

}

bool DirectX11Graphics::InitializeScene()//VertexBuffer.h �ۼ��� �߰�
{
	try
	{
		//HRESULT hr = texVertexBuffer.Initialize(device.Get(), vertices, vertexCount);
		//HRESULT hr = modelVertexBuffer.Initialize(device.Get(), vertices, vertexCount);
		HRESULT hr = constantMatricesBuffer.Initialize(device.Get(), deviceContext.Get()); //��� ��� �����͸� ���ۿ� �ʱ�ȭ
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize world constatnt buffer.");

		 hr = constantLightBuffer.Initialize(device.Get(), deviceContext.Get()); // ���� ��� �����͸� ���ۿ� �ʱ�ȭ 
		COM_ERROR_IF_FAILED(hr, "Failed to initialize  directional light constant buffer.");

		//texture = std::make_unique<Texture>(device.Get(), "Textures\\box.jpg");//�ؽ��� ���ҽ� �̹����� load

		cameraObj.AddComponent<DXCamera>()->SetProjection(screenWidth, screenHeight, 0.1f, 1000.0f);
		cameraObj.GetTransform()->SetPosition({ 0.0f,1.5f,-50.0f });
		cameraObj.GetTransform()->SetLookAt({ 0.0f,0.0f,0.0f });

		//������ ������ ��� ���۵��� mesh�� ���� �� mesh�� �����.
		//primitive = std::make_unique<PrimitiveCube>(); // ť�� ��� mesh

		XMFLOAT3 add_pos;

		//�¾�
		parentPrimitive = std::make_unique<PrimitiveSphere>(); // �� ��� mesh
		parentPrimitive->Set_size(10);
		parentPrimitive->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		parentPrimitive->MakePrimitive("Textures\\sun1024.jpg"); // mesh�� �ؽ��� �̹��� �߰�
		parentPrimitiveObj.AddComponent<DXMeshRenderer>()->SetModel(parentPrimitive.get());

		//�¾� - ? - ���� �� ?��� -? ������ ���� ���
		Sun_Earth_node.GetTransform()->SetParent(parentPrimitiveObj.GetTransform());
		
		//����
		Primitive = std::make_unique<PrimitiveSphere>(); // �� ��� mesh
		Primitive->Set_size(3);
		Primitive->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		Primitive->MakePrimitive("Textures\\earth2048.bmp"); // mesh�� �ؽ��� �̹��� �߰�
		PrimitiveObj.AddComponent<DXMeshRenderer>()->SetModel(Primitive.get());

		//������ �θ� <�¾�> Set
		add_pos = { 25.0f,0,0 };
		PrimitiveObj.GetTransform()->AdjustPosition(add_pos);
		PrimitiveObj.GetTransform()->SetParent(Sun_Earth_node.GetTransform());

		//���� - ? - ���� �� ?��� -? ���� ���� ��� 
		Earth_Moon_node.GetTransform()->SetParent(PrimitiveObj.GetTransform());

		//��
		childPrimitive = std::make_unique<PrimitiveSphere>(); // �� ��� mesh
		childPrimitive->Set_size(1);
		childPrimitive->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		childPrimitive->MakePrimitive("Textures\\moon1024.bmp"); // mesh�� �ؽ��� �̹��� �߰�
		childPrimitiveObj.AddComponent<DXMeshRenderer>()->SetModel(childPrimitive.get());

		//���� �θ� <����> Set
		add_pos = { 5.0f,0,0 };
		childPrimitiveObj.GetTransform()->AdjustPosition(add_pos);
		childPrimitiveObj.GetTransform()->SetParent(Earth_Moon_node.GetTransform());


		///*TEST*/
		//primitive_test = std::make_unique<PrimitiveSphere>(); // �� ��� mesh
		//primitive_test->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		//primitive_test->MakePrimitive("Textures\\earth2048.bmp"); // mesh�� �ؽ��� �̹��� �߰�
		//primitiveObj_test.AddComponent<DXMeshRenderer>()->SetModel(primitive_test.get());
		///**/
		frustum.Initialize(cameraObj.GetComponent<DXCamera>());
	}
	catch (COMException& exception)
	{
		Log::ErrorMsg(exception);
		return false;
	}
	return true;
}



//==========================Mesh ���� �� �ڵ� =================================
//void DirectX11Graphics::RenderFrame() // winapi�� ������۸��� ����(back���ۿ� �׸��Ÿ� ���� ȭ�鿡 �ű�� ����) 
//{
//
//	float color[4] = { 0.0f, 0.0f ,1.0f ,1.0f }; // {R,G,B,����}
//	//�� ����(back surface)�� �����.
//	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
//	//TODO CODE
//	/* z / stencil buffer ���� */
//	//z / Stencilbuffer�� �����.
//	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
//	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
//	/**/
//
//	/*VertexBuffer.h �ۼ��� �߰�*/
//	//���̴� ����.
//	deviceContext->IASetInputLayout(vertexShader.GetInputLayOut());
//	deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
//	deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
//
//	/* z / stencil buffer ���� */
//	constantMatricesBuffer.data.world = DirectX::XMMatrixIdentity();
//	constantMatricesBuffer.data.viewProjection = cameraObj.GetComponent<DXCamera>()->GetViewProjectioMatrix();
//	if (constantMatricesBuffer.ApplyChanges()) deviceContext->VSSetConstantBuffers(0, 1, constantMatricesBuffer.GetAddressOf());
//	/**/
//
//	//Texture Ŭ���� �ۼ��� �߰� 
//	deviceContext->PSSetShaderResources(0, 1, texture->GetTextureResourceViewAddress());
//	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
//
//	/* ���� �۾� �߰� */
//	constantLightBuffer.data.material.ambient = { 1.0f, 1.0f, 1.0f, 1.0f }; // ȯ�汤(���)
//	constantLightBuffer.data.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���ݻ籤(���)
//	constantLightBuffer.data.material.specular = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���ݻ籤(���)
//	constantLightBuffer.data.material.specularPower = 50.0f; //���ݻ籤�� ��ī�ο�(����)
//
//	//���� �������� �ʴ� ���� ��� ������ ĥ����, ��ü�� ���� �� �� ���� ������ ���� ȯ�汤�� ���� (0,0,0)�� ������� �ʴ´�.
//	constantLightBuffer.data.globalAmbient = { 0.1f, 0.1f, 0.1f, 1.0f };// ���� ȯ�汤(������)
//	constantLightBuffer.data.light_color = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���� ��(���)
//
//	XMStoreFloat4(&constantLightBuffer.data.light_direction, XMVector3Normalize(XMVectorSet(light_dir_x, light_dir_y, light_dir_z, 0.0f)));//���� ����
//	XMStoreFloat4(&constantLightBuffer.data.camPosition, cameraObj.GetTransform()->GetPosition()); // ī�޶��� ��ġ .
//	if (constantLightBuffer.ApplyChanges()) deviceContext->PSSetConstantBuffers(0, 1, constantLightBuffer.GetAddressOf());
//	/**/
//
//	//���� ���� ����
//	UINT offsets = 0;
//	//IA(Input Assbler�� vertexbuffer�� ����(Set)) 
//	//deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(),&offsets);
//	// �����
//	//Texture Ŭ���� �ۼ��� ���� (vertexbufer���� �ؽ��� ������ ���� texVertexbuffer�� ��ü)
//	//IA(Input Assbler�� texVertexbuffer(�ؽ��� vertex)�� ����(Set)) 
//	//deviceContext->IASetVertexBuffers(0, 1, texVertexBuffer.GetAddressOf(), texVertexBuffer.StridePtr(), &offsets);
//	//IA �ؽ��Ĺ��� -> ��������� Texture ���۷� ����<ModelVertex> modelVertexBuffer
//	deviceContext->IASetVertexBuffers(0, 1, modelVertexBuffer.GetAddressOf(), modelVertexBuffer.StridePtr(), &offsets);
//	//���� ���۸� �̿��Ͽ� �׸���.
//	//deviceContext->Draw(vertexBuffer.VertexCount(), 0); //IndexBuffer.h �ۼ��� �ּ�ó��
//
//	//�ε��� ���� ����, DWORD(unsigned Long) : R32_UINT
//	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
//	//�ε��� ���۸� �̿��Ͽ� �׸��� �׸���. 
//	deviceContext->DrawIndexed(indexBuffer.IndexCount(), 0, 0);
//
//	//������۸��� ���� ȭ�鿡 ����ϴ� �Լ� 
//	//������ winapi�� �Ȱ����� �ƴϰ� front buffer <> back buffer �� ����� ���
//	//https://hannom.tistory.com/156https://hannom.tistory.com/156 (����)
//	swapChain->Present(NULL, NULL); // Present ���ڰ��� UINT SyncInterval �� ��������ȭ �����̸� , ������ �Ű������� ���� ��������ȭ�� ���۵ȴ�.
//	//��������ȭ�� ���Ǵ� ������ scd.BufferDesc.RefreshRate.Numerator �� �����͸� ����Ͽ� �����Ѵ�.
//
//}


//	//	VertexColor vertices[] = {
//	//		//Vertexbuffer���� ����߾��� (TEST)
//	//		/*
//	//		VertexColor(0.0f,0.1f,0.0f,1.0f,0.0f,0.0f), // top middle (red)
//	//		VertexColor(0.1f,-0.1f,0.0f,0.0f,1.0f,0.0f), // bottom right (green)
//	//		VertexColor(-0.1f,-0.1f,0.0f,0.0f,1.0f,1.0f), // bottom left (cyan)
//	//		*/
//
//	//		//indexbuffer���� �����(TEST)
//	//		/*
//	//		VertexColor(-0.3f,  0.3f, 0.0f,  1.0f, 0.0f, 0.0f), // 0 top left red
//	//		VertexColor(0.3f,   0.3f, 0.0f,  0.0f, 1.0f, 0.0f), // 1 top right green
//	//		VertexColor(0.3f,  -0.3f, 0.0f,  0.0f, 1.0f, 1.0f), // 2 bottom right cyan
//	//		VertexColor(-0.3f, -0.3f, 0.0f,  1.0f, 1.0f, 0.0f), // 3 bottom left yellow
//	//		*/
//
//	//		//3D���������� ���� (TEST)
//	//		
//	//		VertexColor(-0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f), // 0 top far left
//	//		VertexColor(0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f), // 1 top far right
//	//		VertexColor(0.5f, 0.5f, -0.5f,		0.0f, 0.0f, 1.0f), // 2 top near right
//	//		VertexColor(-0.5f, 0.5f, -0.5f,		1.0f, 1.0f, 0.0f), // 3 top near left
//
//	//		VertexColor(-0.5f, -0.5f, 0.5f,		0.0f, 1.0f, 1.0f), // 4 bottom far left
//	//		VertexColor(0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 1.0f), // 5 bottom far right
//	//		VertexColor(0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 1.0f), // 6 bottom near right
//	//		VertexColor(-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f), // 7 bottom near left
//	//		
//	//}
//
//	//VertexTexture vertices[] = 
//	//{
//	//	//vertex ��ǥ (x,y,z)		UV��ǥ���� ��ǥ( u,v)
//	//	VertexTexture(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f), // 0 top far left
//	//	VertexTexture(0.5f, 0.5f, 0.5f, 1.0f, 0.0f), // 1 top far right
//	//	VertexTexture(0.5f, 0.5f, -0.5f, 1.0f, 1.0f), // 2 top near right
//	//	VertexTexture(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f), // 3 top near left
//	//	VertexTexture(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f), // 4 bottom far left
//	//	VertexTexture(0.5f, -0.5f, 0.5f, 1.0f, 1.0f), // 5 bottom far right
//	//	VertexTexture(0.5f, -0.5f, -0.5f, 1.0f, 0.0f), // 6 bottom near right
//	//	VertexTexture(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f), // 7 bottom near left
//	//	VertexTexture(0.5f, -0.5f, 0.5f, 0.0f, 0.0f), // 8 other bottom far right
//	//	VertexTexture(0.5f, -0.5f, -0.5f, 0.0f, 1.0f), // 9 other bottom near right
//	//	VertexTexture(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f), // 10 other bottom near left
//	//	VertexTexture(-0.5f, -0.5f, 0.5f, 1.0f, 0.0f), // 11 other bottom far left  
//	//};
//
//		//x,y,z(vertex�� ���� ��ǥ) U,V(UV��ǥ�� �ؽ��� ��ǥ) nx,ny,nz(�븻(����)����)
//ModelVertex vertices[] =
//{
//ModelVertex(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f), // 0 top far left
//ModelVertex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f), // 1 top far right
//ModelVertex(0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f), // 2 top near right
//ModelVertex(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f), // 3 top near left
//
//ModelVertex(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f), // 4 bottom far left
//ModelVertex(0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f), // 5 bottom far right
//ModelVertex(0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f), // 6 bottom near right
//ModelVertex(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f), // 7 bottom near left
//
//ModelVertex(0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f), // 8 other bottom far right
//ModelVertex(0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f), // 9 other bottom near right
//ModelVertex(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f), // 10 other bottom near left
//ModelVertex(-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f), // 11 other bottom far left
//};
// 
//// 		//Vertex�׸��� ���� ,�ε������۰� �� ���� ����
//DWORD indices[] =
//{
//	//indexbuffer���� �����(TEST)
//	/*
//	0, 1, 2,
//	0, 2, 3
//	*/
//
//	//3D���������� ���� (TEST)
//	/*
//	0, 1, 2,	 0, 2, 3, // top
//	7, 6, 5,	 7, 5, 4, // bottom
//	3, 2, 6,	 3, 6, 7, // front
//	1, 0, 4,	 1, 4, 5, // back
//	2, 1, 5,	 2, 5, 6, // right
//	0, 3, 7,	 0, 7, 4, // left
//	*/
//
//	0, 1, 2,	0, 2, 3, // top
//	7, 6, 5,	7, 5, 4, // bottom
//	3, 2, 6,	3, 6, 7, // front
//	1, 0, 4,	1, 4, 5, // back
//	2, 1, 8,	2, 8, 9, // right
//	0, 3, 10,	0, 10, 11, // left
//
//};
// 
// 	
///* ����(��) ���� ���� */
//hr = constantLightBuffer.Initialize(device.Get(), deviceContext.Get());
//COM_ERROR_IF_FAILED(hr, "Failed to initialize directional light constant buffer.");
///**/
//
///* z / stencil buffer ���� */
//hr = constantMatricesBuffer.Initialize(device.Get(), deviceContext.Get());
//COM_ERROR_IF_FAILED(hr, "Failed to Initialize world/view/projection constant buffer.");
//=======================================================================================