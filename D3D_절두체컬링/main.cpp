//main.cpp
#include"System.h" // System 클래스 작성 후 추가 하였음 
#include"DirectX11Graphics.h" //DirectX11Graphics.h 클래스 작성후 추가 하였음 

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevlnstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// System 클래스 작성 후 추가 하였음 
	std::unique_ptr<System> main = std::make_unique<System>();
	//if (main->Intialize(hInstance, L"DirectX11", 800, 600)) // 창모드 800*600
	if (main->Intialize<DirectX11Graphics>(hInstance, L"DirectX11", 800, 600)) // DirectX11Graphics.h 클래스 작성후 추가 하였음 
	{
		//true인경우 게임루프로 진입 
		main->Processing();
	}

	return 0;
}