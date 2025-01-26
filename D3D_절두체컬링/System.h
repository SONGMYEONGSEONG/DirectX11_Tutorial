#pragma once
#include "WindowContainer.h"
class System final : public WindowContainer
{
public:
	//is_base_of : Base로부터 상속된 class가 맞는지 확인하는 코드 ex) Graphics(부모)로 상속된 T(자식 타입)인경우 True 리턴
	//enable_if : 예제 코드 확인 
	/*
	template <typename T>
	struct enable_if<true, T> {
		typedef T type; // 참이면 해당 클래스를 T로 초기화 
	};
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Graphics, T>::value>>// DirectX11 초기화 작업 -  추가

	bool Intialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height, bool fullScreen = false)
	{
		window = std::make_unique<Window>();
		if (!window->Intialize(this, hInstance, title, width, height, fullScreen)) return false;

		Input::Initialize(window->GetScreenWidth(), window->GetScreenHeight());
		timer = std::make_unique<Timer>();
		timer->Start();

		// DirectX11 초기화 작업 -  추가
		gfx = std::make_unique<T>();
		if (!gfx->Initialize(this)) return false;
		//

		return true;	
	}

	void Processing();

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual bool Update() override;
	virtual void RenderFrame() override;
};

