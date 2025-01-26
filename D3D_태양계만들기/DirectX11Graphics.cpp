#include "DirectX11Graphics.h"

bool DirectX11Graphics::Initialize(WindowContainer* pWindowContainer)
{
	Window* pWindow = pWindowContainer->GetWindow();
	screenWidth = pWindow->GetScreenWidth();
	screenHeight = pWindow->GetScreenHeight();

	if (!InitializeDirectX(pWindow->GetHWND(), pWindow->IsFullScreen())) return false;

	timer = pWindowContainer->GetTimer();

	if (!InitializeShader()) return false;//VertexBuffer.h 작성후 추가
	if (!InitializeScene()) return false;//VertexBuffer.h 작성후 추가

	return true;
}

void DirectX11Graphics::Update()
{
	const float deltaTime = timer->GetMilisecondsElapesed();
	static float earthYaw = 0, SunYaw =0 , MoonYaw =0;

	earthYaw += deltaTime * 0.1f;
	SunYaw += deltaTime * 0.05f;
	MoonYaw += deltaTime * 0.1f;
	parentPrimitiveObj.GetTransform()->SetLocalRotation({ 0, SunYaw, 0 }); // 태양
	Sun_Earth_node.GetTransform()->SetLocalRotation({ 0, SunYaw, 0 }); // 지구 공전
	PrimitiveObj.GetTransform()->SetLocalRotation({ 0,earthYaw, 0 });  // 지구
	Earth_Moon_node.GetTransform()->SetLocalRotation({ 0, MoonYaw, 0 }); // 달 공전
	childPrimitiveObj.GetTransform()->SetLocalRotation({ 0, MoonYaw, 0 }); // 달

	/* z / stencil buffer 구현 */
	static float accel = 1.0f; //가속도
	//const float deltaTime = timer->GetMilisecondsElapesed();
	static int light_reverse_num = 1; //빛이 방향 부호값
	if (Input::GetKeyboard()->IsDown(VK_SHIFT))
	{
		accel = 5.0f; // Shift key Down, 이동 5배 가속
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

	//카메라 WASD(상하좌우) 이동
	if (Input::GetKeyboard()->IsPressed('D')) cameraObj.GetTransform()->AdjustPosition(cameraObj.GetTransform()->GetRight() * deltaTime * 0.01f * accel);
	if (Input::GetKeyboard()->IsPressed('A')) cameraObj.GetTransform()->AdjustPosition(-cameraObj.GetTransform()->GetRight() * deltaTime * 0.01f * accel);
	if (Input::GetKeyboard()->IsPressed('W')) cameraObj.GetTransform()->AdjustPosition(cameraObj.GetTransform()->GetUp() * deltaTime * 0.01f * accel);
	if (Input::GetKeyboard()->IsPressed('S')) cameraObj.GetTransform()->AdjustPosition(-cameraObj.GetTransform()->GetUp() * deltaTime * 0.01f * accel);
	

	//TEST - 빛의 방향 변경
	// 왼손좌표계기준
	//빛의 방향 =/= 광원 , 빛의 방향(위치 x, 방향(벡터) O) 
	if (Input::GetKeyboard()->IsPressed('Q'))
	{
		//빛의 방향이 오른쪽으로 이동(회전), spceular는 정반대로 이동하는것처럼 보일것
		//정상임
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
		case MouseEventType::WheelVertical: //카메라 앞(Wheel up) / 뒤 (Wheel down) 이동.
		{
			cameraObj.GetTransform()->AdjustPosition(cameraObj.GetTransform()->GetForward() * deltaTime * mouseEvent.y * accel);
			break;
		}
		case MouseEventType::RawMove:
		{
			if (Input::GetMouse()->IsDown(ButtonType::Right))//카메라 x,y축 회전(마우스 오른쪽 클릭 & 마우스 이동)
			{
				XMVECTOR v = XMVectorSet(mouseEvent.y, mouseEvent.x, 0.0f, 0.0f) * deltaTime * 0.1f;
				cameraObj.GetTransform()->AdjustRotation(v);
			}
			break;
		}

	}
	/**/
}

//Mesh 추가후 코드 정리 
void DirectX11Graphics::RenderFrame() // winapi의 더블버퍼링과 유사(back버퍼에 그린거를 실제 화면에 옮기는 역할) 
{

	float color[4] = { 0.0f, 0.0f ,1.0f ,1.0f }; // {R,G,B,투명도}
	//백 버퍼(back surface)를 지운다.
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	//z / Stencilbuffer를 지운다.
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);

	//TODO CODE
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);//z,스텐실 버퍼 설정 OM(출력 병합기)에 세팅
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf()); //PS에 랜더링 샘플러 세팅(address code)

	/* 조명 추가 */
	//material이 갖고있는 조명 반사색 (mesh가 갖고있는 텍스쳐 색 , material 변수가 갖고있는 조명data) 
	constantLightBuffer.data.material.ambient = { 1.0f, 1.0f, 1.0f, 1.0f }; // 환경광(흰색)
	constantLightBuffer.data.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // 난반사광(흰색)
	constantLightBuffer.data.material.specular = { 1.0f, 1.0f, 1.0f, 1.0f }; // 정반사광(흰색)
	constantLightBuffer.data.material.specularPower = 50.0f; //정반사광의 날카로움(세기)

	//빛이 비춰지지 않는 면은 새까만 색으로 칠해져, 무체를 구별 할 수 없기 때문에 전역 환경광은 검정 (0,0,0)을 사용하지 않는다.
	//light가 갖고있는 반사색 
	constantLightBuffer.data.globalAmbient = { 0.1f, 0.1f, 0.1f, 1.0f };// 전역 환경광(검정색)
	constantLightBuffer.data.light_color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 빛의 색(흰색)
	//정규화를 계속해도 크기는 1이기 때문에 결국 빛의 속도(조작)는 줄어든다.
	// 1 -> 100 - 1000 -> .... 값이 커질수록 정규화로 크기는 1을 유지하기떄문에 
	XMStoreFloat4(&constantLightBuffer.data.light_direction, XMVector3Normalize(XMVectorSet(light_dir_x, light_dir_y, light_dir_z, 0.0f)));//빛의 방향
	XMStoreFloat4(&constantLightBuffer.data.camPosition, cameraObj.GetTransform()->GetPosition()); // 카메라의 위치 .
	if (constantLightBuffer.ApplyChanges()) deviceContext->PSSetConstantBuffers(0, 1, constantLightBuffer.GetAddressOf());
	/**/

	//셰이더 적용.
	deviceContext->IASetInputLayout(vertexShader.GetInputLayOut());
	deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);

	frustum.ConstructFrustum();//절두체 컬링 작업 추가 
	//GameObject Primitive Mesh Draw
	if (frustum.IsInFrustumBoundsCube(parentPrimitiveObj.GetTransform()->GetPosition(), XMVectorGetX(parentPrimitiveObj.GetTransform()->GetScale())/*bounds*/))//절두체 컬링 작업 추가 
		parentPrimitiveObj.Draw(cameraObj.GetComponent<DXCamera>()->GetViewProjectionMatrix());

	if (frustum.IsInFrustumBoundsCube(PrimitiveObj.GetTransform()->GetPosition(), XMVectorGetX(PrimitiveObj.GetTransform()->GetScale())/*bounds*/))//절두체 컬링 작업 추가 
		PrimitiveObj.Draw(cameraObj.GetComponent<DXCamera>()->GetViewProjectionMatrix());

	if (frustum.IsInFrustumBoundsCube(childPrimitiveObj.GetTransform()->GetPosition(), XMVectorGetX(childPrimitiveObj.GetTransform()->GetScale())/*bounds*/))//절두체 컬링 작업 추가 
		childPrimitiveObj.Draw(cameraObj.GetComponent<DXCamera>()->GetViewProjectionMatrix());

	swapChain->Present(NULL, NULL);
}

bool DirectX11Graphics::InitializeDirectX(HWND hWnd, bool fullScreen)
{
	try
	{
		// Swap Chain 생성에 사용될 Description 구조체 설정
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = screenWidth; // 백 버퍼의 크기 설정
		scd.BufferDesc.Height = screenHeight;
		scd.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

		// (0 / 1) : 1초에 최대한 빨리 그리게 한다.(v_sync 관련내용)
		//0/1은 모니터마다 주사율을 고려하지않고 화면갱신을 하라는 뜻이고 현재 이값은 바꿔도 수직동기화는 적용되지 않는다
		//수직동기화를 사용하려면 swapchain의 present함수에서 활성화해야되며 , 모니터에 Hz값 및 각종데이터를 사용하여 수직동기화 처리르 한다. 
		scd.BufferDesc.RefreshRate.Numerator = 0;//분자 
		scd.BufferDesc.RefreshRate.Denominator = 1;//분모(절대 0이 되면 안됨)

		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 순차 주사 대 비월 주사 (지정하지 않음)
		//비월 주사 : 짝수,홀수주 주사선을 번갈아가며 출력하는 방식 ex) 480i,720i...
		//순차 주사: 주사선을 1초에 60번씬 한꺼번에 출력하는 방식( Hz) ex) 480p,720p...
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;//이미지를 모니터에 맞게 확대/축소 하는 방식(지정하지 않음) 
	
		//MSAA(Multi Sample Anti Aliasing)를 사용하지 않음
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //백버퍼 사용처(용도) 설정.
		scd.BufferCount = 1; // 백 버퍼를 1개 사용.

		scd.OutputWindow = hWnd; // 렌더링이 이루어질 윈도우(의 핸들)를 설정.
		scd.Windowed = !fullScreen;

		scd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; // 백 버퍼 내용을 사용(present, 화면에 출력)후 내용을 버린다(discard)
		scd.Flags = 0;

		//feature level : 어느 버전의 DirectX를 사용할 것인가.
		//사양이 낮은 컴퓨터를 지원하기 위하여 하위 버전의 DirectX 버전을 추가.
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
		};
		UINT featureLevelCount = ARRAYSIZE(featureLevels);

		// ========== DirectX11 Device, Device Context, Swap Chain 생성 ==========
		// DirectX11 Device : 개체를 할당 및 삭제하며 기본 형식을 렌더링하며 그래픽 드라이버 및 하드웨어와 통신

		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
			NULL, // D3D_DRIVER_TYPE_SOFTWARE가 아니라면 반드시 NULL 이어야 하고 아니면 NULL이면 안된다.
			NULL, // 런타임 계층에 대한 flags
			featureLevels,
			featureLevelCount,
			D3D11_SDK_VERSION,
			&scd,
			swapChain.GetAddressOf(),
			device.GetAddressOf(),
			NULL,//지원되는 기능 수준(feature level) output
			deviceContext.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create device and swap chain.");
		// ===================================================================

		// =========================== 백 버퍼 생성 ==============================
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		//SwapChain을 이용하여 백 버퍼를 가져온다.
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to GetBuffer.");
		//백 버퍼를 RenderTargetView에 연결.
		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");
		// =======================================================================

		// ============================ 뷰포트 설정 ==============================
		// 뷰포트 설정을 변경하지 않을 경우, 이 작업은 생략 가능.
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight));
		deviceContext->RSSetViewports(1, &viewport); //NumViewports(1) : 뷰 포트의 수.( 뷰포트 배열을 파이프라인의 래스터라이즈 단계에 바인딩)
		// =======================================================================

		// ========================= 래스터라이즈 생성 ===========================
		// Rasterizer State는 직접 설정하지 않아도 기본적으로 설정되어 있다. 다만 제어 권한이 없다.
		CD3D11_RASTERIZER_DESC rasterDesc(D3D11_DEFAULT);
		// rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; 면을 색으로 채우기.
		// rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK; 후면 컬링.
		// rasterDesc.FrontCounterClockwise = false; 반시계방향(Counter Clock wise) 그리기를 뒷면으로.
		hr = device->CreateRasterizerState(&rasterDesc, rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
		// ======================================================================

		/* z / stencil buffer 구현 */
		// ========================= 깊이/스탠실 State 생성 ===========================
		//깊이 버퍼 사용O, 스텐실 버퍼 사용X
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
		// ======================================================================

		// ========================= 깊이/스탠실 buffer 생성 ===========================
		//Mip Level : 밉맵, 동일한 그림을 다양한 사이즈로 만드는 것, 렌더링의 속도를 높이고 앨리어싱(계단현상)을 줄여준다.
		CD3D11_TEXTURE2D_DESC depthBufferDesc(DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT, screenWidth, screenHeight);
		depthBufferDesc.MipLevels = 1;
		//깊이/스텐실 버퍼로 사용할 것 이기 때문에 depth/Stencil로 bind
		depthBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		hr = device->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to Create depth stencil view.");
		// ======================================================================
		/**/

		//Texture 클래스 작성후 추가
		// ========================= 샘플러 State 생성 ===========================
		//Texture address mode 꼭 알고있을것 (기본중의 기본)
		//모드를 설정할때 u,v,w(x,y,z)를 다 똑같이 설정해주면 된다.
		//메쉬에 텍스쳐를 입히는(색으 칠하는) 과정이라 레스터라이즈 단계에서 실행된다.(레스터라이즈 밑으로 작성하면됨)
		CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
		// ======================================================================

		// NumViews(1) : 스왑 체인을 만들 때 백 버퍼를 1개 사용으로 하였기 때문에,
		//				 랜더 타겟 뷰 하나만 사용.
		//NULL -> depthStencilView.Get() 로 변경 ( z / stencil buffer 구현)
		deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get()); // OM -> OutputMerger : Z-buufer,stencil-buffer 등 병합출력 하는곳
		deviceContext->RSSetState(rasterizerState.Get()); // RS ->Resterize의 약자

		//기본 도형 위상구조를 사용하여 기본 도형을 어떻게 그리게할지 설정 (삼각형으로 설정)
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//vertexBuffer.h 작성후 추가

	}//try
	catch (COMException& exception)
	{
		Log::ErrorMsg(exception);
		return false;
	}
	return true;
}

bool DirectX11Graphics::InitializeShader()//VertexBuffer.h 작성후 추가
{
	//Shader? : 3D컴퓨터 그래픽에서 최종적으로 화면에 출력하는 픽셀의 색을 정해주는 함수 
	D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0}, // vertexShader에서 vertex 정보를 가지고 좌표를 설정(공간변환,행렬등등)
		//텍스쳐 깨지는 이유: 텍스쳐 메모리 접근 잘못함, A32를 적었어야함(texture.hlsl이 flolat4로 사용하기 떄문에) 
		{ "COLOR",0,DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0}, // 레스터라이드 단계에서 가져온 데이터를 PixelShader에서 각 픽셀마다 데이터를 생성
		{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 텍스쳐 2d이미자가 추가되면서 메쉬에 입힐 텍스쳐 쉐이더가 필요해서 생성
		{"NORMAL",0,DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0},
		//조명이 적용된 텍스쳐 작업을하면서 조명 쉐이더가 필요함 -> 조명(노말벡터 필요) 노말쉐이더 
	};
	UINT numElements = ARRAYSIZE(inputLayout);
	if (!vertexShader.Initialize(device.Get(), L"light.vs.hlsl", inputLayout, numElements)) return false;
	if (!pixelShader.Initialize(device.Get(), L"light.ps.hlsl")) return false;
	return true;

}

bool DirectX11Graphics::InitializeScene()//VertexBuffer.h 작성후 추가
{
	try
	{
		//HRESULT hr = texVertexBuffer.Initialize(device.Get(), vertices, vertexCount);
		//HRESULT hr = modelVertexBuffer.Initialize(device.Get(), vertices, vertexCount);
		HRESULT hr = constantMatricesBuffer.Initialize(device.Get(), deviceContext.Get()); //상수 행렬 데이터를 버퍼에 초기화
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize world constatnt buffer.");

		 hr = constantLightBuffer.Initialize(device.Get(), deviceContext.Get()); // 조명 상수 데이터를 버퍼에 초기화 
		COM_ERROR_IF_FAILED(hr, "Failed to initialize  directional light constant buffer.");

		//texture = std::make_unique<Texture>(device.Get(), "Textures\\box.jpg");//텍스쳐 리소스 이미지를 load

		cameraObj.AddComponent<DXCamera>()->SetProjection(screenWidth, screenHeight, 0.1f, 1000.0f);
		cameraObj.GetTransform()->SetPosition({ 0.0f,1.5f,-50.0f });
		cameraObj.GetTransform()->SetLookAt({ 0.0f,0.0f,0.0f });

		//위에서 세팅한 상수 버퍼들을 mesh에 적용 후 mesh를 만든다.
		//primitive = std::make_unique<PrimitiveCube>(); // 큐브 모양 mesh

		XMFLOAT3 add_pos;

		//태양
		parentPrimitive = std::make_unique<PrimitiveSphere>(); // 구 모양 mesh
		parentPrimitive->Set_size(10);
		parentPrimitive->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		parentPrimitive->MakePrimitive("Textures\\sun1024.jpg"); // mesh에 텍스쳐 이미지 추가
		parentPrimitiveObj.AddComponent<DXMeshRenderer>()->SetModel(parentPrimitive.get());

		//태양 - ? - 지구 의 ?노드 -? 지구의 공전 노드
		Sun_Earth_node.GetTransform()->SetParent(parentPrimitiveObj.GetTransform());
		
		//지구
		Primitive = std::make_unique<PrimitiveSphere>(); // 구 모양 mesh
		Primitive->Set_size(3);
		Primitive->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		Primitive->MakePrimitive("Textures\\earth2048.bmp"); // mesh에 텍스쳐 이미지 추가
		PrimitiveObj.AddComponent<DXMeshRenderer>()->SetModel(Primitive.get());

		//지구의 부모를 <태양> Set
		add_pos = { 25.0f,0,0 };
		PrimitiveObj.GetTransform()->AdjustPosition(add_pos);
		PrimitiveObj.GetTransform()->SetParent(Sun_Earth_node.GetTransform());

		//지구 - ? - 달의 의 ?노드 -? 달의 공전 노드 
		Earth_Moon_node.GetTransform()->SetParent(PrimitiveObj.GetTransform());

		//달
		childPrimitive = std::make_unique<PrimitiveSphere>(); // 구 모양 mesh
		childPrimitive->Set_size(1);
		childPrimitive->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		childPrimitive->MakePrimitive("Textures\\moon1024.bmp"); // mesh에 텍스쳐 이미지 추가
		childPrimitiveObj.AddComponent<DXMeshRenderer>()->SetModel(childPrimitive.get());

		//달의 부모를 <지구> Set
		add_pos = { 5.0f,0,0 };
		childPrimitiveObj.GetTransform()->AdjustPosition(add_pos);
		childPrimitiveObj.GetTransform()->SetParent(Earth_Moon_node.GetTransform());


		///*TEST*/
		//primitive_test = std::make_unique<PrimitiveSphere>(); // 구 모양 mesh
		//primitive_test->Initialize(device.Get(), deviceContext.Get(), constantMatricesBuffer);
		//primitive_test->MakePrimitive("Textures\\earth2048.bmp"); // mesh에 텍스쳐 이미지 추가
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



//==========================Mesh 수정 전 코드 =================================
//void DirectX11Graphics::RenderFrame() // winapi의 더블버퍼링과 유사(back버퍼에 그린거를 실제 화면에 옮기는 역할) 
//{
//
//	float color[4] = { 0.0f, 0.0f ,1.0f ,1.0f }; // {R,G,B,투명도}
//	//백 버퍼(back surface)를 지운다.
//	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
//	//TODO CODE
//	/* z / stencil buffer 구현 */
//	//z / Stencilbuffer를 지운다.
//	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
//	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
//	/**/
//
//	/*VertexBuffer.h 작성후 추가*/
//	//셰이더 적용.
//	deviceContext->IASetInputLayout(vertexShader.GetInputLayOut());
//	deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
//	deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
//
//	/* z / stencil buffer 구현 */
//	constantMatricesBuffer.data.world = DirectX::XMMatrixIdentity();
//	constantMatricesBuffer.data.viewProjection = cameraObj.GetComponent<DXCamera>()->GetViewProjectioMatrix();
//	if (constantMatricesBuffer.ApplyChanges()) deviceContext->VSSetConstantBuffers(0, 1, constantMatricesBuffer.GetAddressOf());
//	/**/
//
//	//Texture 클래스 작성후 추가 
//	deviceContext->PSSetShaderResources(0, 1, texture->GetTextureResourceViewAddress());
//	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
//
//	/* 조명 작업 추가 */
//	constantLightBuffer.data.material.ambient = { 1.0f, 1.0f, 1.0f, 1.0f }; // 환경광(흰색)
//	constantLightBuffer.data.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // 난반사광(흰색)
//	constantLightBuffer.data.material.specular = { 1.0f, 1.0f, 1.0f, 1.0f }; // 정반사광(흰색)
//	constantLightBuffer.data.material.specularPower = 50.0f; //정반사광의 날카로움(세기)
//
//	//빛이 비춰지지 않는 면은 새까만 색으로 칠해져, 무체를 구별 할 수 없기 때문에 전역 환경광은 검정 (0,0,0)을 사용하지 않는다.
//	constantLightBuffer.data.globalAmbient = { 0.1f, 0.1f, 0.1f, 1.0f };// 전역 환경광(검정색)
//	constantLightBuffer.data.light_color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 빛의 색(흰색)
//
//	XMStoreFloat4(&constantLightBuffer.data.light_direction, XMVector3Normalize(XMVectorSet(light_dir_x, light_dir_y, light_dir_z, 0.0f)));//빛의 방향
//	XMStoreFloat4(&constantLightBuffer.data.camPosition, cameraObj.GetTransform()->GetPosition()); // 카메라의 위치 .
//	if (constantLightBuffer.ApplyChanges()) deviceContext->PSSetConstantBuffers(0, 1, constantLightBuffer.GetAddressOf());
//	/**/
//
//	//정점 버퍼 적용
//	UINT offsets = 0;
//	//IA(Input Assbler에 vertexbuffer를 설정(Set)) 
//	//deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(),&offsets);
//	// ▼수정
//	//Texture 클래스 작성후 변경 (vertexbufer에서 텍스쳐 맵핑을 위한 texVertexbuffer로 교체)
//	//IA(Input Assbler에 texVertexbuffer(텍스쳐 vertex)를 설정(Set)) 
//	//deviceContext->IASetVertexBuffers(0, 1, texVertexBuffer.GetAddressOf(), texVertexBuffer.StridePtr(), &offsets);
//	//IA 텍스쳐버퍼 -> 조명적용된 Texture 버퍼로 변경<ModelVertex> modelVertexBuffer
//	deviceContext->IASetVertexBuffers(0, 1, modelVertexBuffer.GetAddressOf(), modelVertexBuffer.StridePtr(), &offsets);
//	//정점 버퍼를 이용하여 그린다.
//	//deviceContext->Draw(vertexBuffer.VertexCount(), 0); //IndexBuffer.h 작성후 주석처리
//
//	//인덱스 버퍼 적용, DWORD(unsigned Long) : R32_UINT
//	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
//	//인덱스 버퍼를 이용하여 그림을 그린다. 
//	deviceContext->DrawIndexed(indexBuffer.IndexCount(), 0, 0);
//
//	//더블버퍼링의 실제 화면에 출력하는 함수 
//	//하지만 winapi와 똑같은건 아니고 front buffer <> back buffer 를 교대로 출력
//	//https://hannom.tistory.com/156https://hannom.tistory.com/156 (참고)
//	swapChain->Present(NULL, NULL); // Present 인자값중 UINT SyncInterval 은 수직동기화 내용이며 , 정수를 매개변수로 사용시 수직동기화가 동작된다.
//	//수직동기화에 사용되는 정보는 scd.BufferDesc.RefreshRate.Numerator 이 데이터를 사용하여 조정한다.
//
//}


//	//	VertexColor vertices[] = {
//	//		//Vertexbuffer에서 사용했었음 (TEST)
//	//		/*
//	//		VertexColor(0.0f,0.1f,0.0f,1.0f,0.0f,0.0f), // top middle (red)
//	//		VertexColor(0.1f,-0.1f,0.0f,0.0f,1.0f,0.0f), // bottom right (green)
//	//		VertexColor(-0.1f,-0.1f,0.0f,0.0f,1.0f,1.0f), // bottom left (cyan)
//	//		*/
//
//	//		//indexbuffer에서 사용함(TEST)
//	//		/*
//	//		VertexColor(-0.3f,  0.3f, 0.0f,  1.0f, 0.0f, 0.0f), // 0 top left red
//	//		VertexColor(0.3f,   0.3f, 0.0f,  0.0f, 1.0f, 0.0f), // 1 top right green
//	//		VertexColor(0.3f,  -0.3f, 0.0f,  0.0f, 1.0f, 1.0f), // 2 bottom right cyan
//	//		VertexColor(-0.3f, -0.3f, 0.0f,  1.0f, 1.0f, 0.0f), // 3 bottom left yellow
//	//		*/
//
//	//		//3D렌더링에서 사용됨 (TEST)
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
//	//	//vertex 좌표 (x,y,z)		UV좌표계의 좌표( u,v)
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
//		//x,y,z(vertex의 월드 좌표) U,V(UV좌표계 텍스쳐 좌표) nx,ny,nz(노말(법선)벡터)
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
//// 		//Vertex그리는 순서 ,인덱스버퍼가 이 값을 저장
//DWORD indices[] =
//{
//	//indexbuffer에서 사용함(TEST)
//	/*
//	0, 1, 2,
//	0, 2, 3
//	*/
//
//	//3D렌더링에서 사용됨 (TEST)
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
///* 조명(빛) 버퍼 구현 */
//hr = constantLightBuffer.Initialize(device.Get(), deviceContext.Get());
//COM_ERROR_IF_FAILED(hr, "Failed to initialize directional light constant buffer.");
///**/
//
///* z / stencil buffer 구현 */
//hr = constantMatricesBuffer.Initialize(device.Get(), deviceContext.Get());
//COM_ERROR_IF_FAILED(hr, "Failed to Initialize world/view/projection constant buffer.");
//=======================================================================================