//main.cpp
#include"System.h" // System Ŭ���� �ۼ� �� �߰� �Ͽ��� 
#include"DirectX11Graphics.h" //DirectX11Graphics.h Ŭ���� �ۼ��� �߰� �Ͽ��� 

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevlnstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// System Ŭ���� �ۼ� �� �߰� �Ͽ��� 
	std::unique_ptr<System> main = std::make_unique<System>();
	//if (main->Intialize(hInstance, L"DirectX11", 800, 600)) // â��� 800*600
	if (main->Intialize<DirectX11Graphics>(hInstance, L"DirectX11", 800, 600)) // DirectX11Graphics.h Ŭ���� �ۼ��� �߰� �Ͽ��� 
	{
		//true�ΰ�� ���ӷ����� ���� 
		main->Processing();
	}

	return 0;
}