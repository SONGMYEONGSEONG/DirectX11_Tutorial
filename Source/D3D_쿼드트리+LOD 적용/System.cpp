#include "System.h"

void System::Processing()
{
	window->Processing();
}

LRESULT System::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Input::MessageHandler(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool System::Update()
{
	if (Input::GetKeyboard()->IsUp(VK_ESCAPE)) return false;
	
	//TODO : Graphics Update
	gfx->Update();// DirectX11 초기화 작업 -  추가
	

	Input::Update();
	timer->Restart();
	return true;
}

void System::RenderFrame()
{
	//TODO : Graphics Render
	gfx->RenderFrame();// DirectX11 초기화 작업 -  추가
}