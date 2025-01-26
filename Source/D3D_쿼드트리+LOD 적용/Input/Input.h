#pragma once
#include <memory>
// <memory> : 개체를 할당하고 해제하는 데 도움이 되는 클래스
#include "Keyboard/InputKeyboard.h"
#include "Mouse/InputMouse.h"

//인풋 매니저 역할
class Input
{
private:
	//unique_ptr ( 유니크 포인터,원시 포인터) <- 스마트 포인터
	//포인터를 단독으로 소유 , 소유권은 이전 가능 ,복사나 대입 불가능 
	//객체가 소멸될때 (소멸자를 호출할때) 포인터도 소멸한다 -> 자동 할당해지 
	static std::unique_ptr<InputKeyboard> keyboard;
	static std::unique_ptr<InputMouse> mouse;

public:
	static void Initialize(UINT screenWidth, UINT screenHeight = 0);

	static InputKeyboard* GetKeyboard() { return keyboard.get(); }
	static InputMouse* GetMouse() { return mouse.get(); }

	static void Update();
	static void MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};