#pragma once
#include <memory>
#include "Window.h"
#include "Input/Input.h"
#include "Timer.h"
#include <hidusage.h> // HID 매크로를 쓰기위해 추가, 반드시 #include <Windows.h>보다 아래 라인에 작성.
#include "Graphics.h" // DirectX11 초기화 작업 -  추가

class WindowContainer abstract
{
protected:
	std::unique_ptr<Window> window;
	std::unique_ptr<Timer> timer;
	std::unique_ptr<Graphics> gfx;// DirectX11 초기화 작업 -  추가

public:
	WindowContainer()
	{
		//Setup Raw Input
		//WM_INPUT을 호출을 하려면 반드시 세팅을 하여아 한다.
		static bool rawInputInitialized = false;
		if (!rawInputInitialized)
		{
			RAWINPUTDEVICE ridev; // RAWINPUTDEVICE : 
			ridev.usUsagePage = HID_USAGE_PAGE_GENERIC; // 일반 데스크탑 컨트롤.
			ridev.usUsage = HID_USAGE_GENERIC_MOUSE; //마우스,
			ridev.dwFlags = 0;
			ridev.hwndTarget = NULL; // NULL이면 키보드 포커스에 따른다.

			// RegisterRawInputDevices : 
			if (!RegisterRawInputDevices(&ridev, 1, sizeof(ridev)))
			{
				Log::ErrorMsg(GetLastError(), "Failed to register raw input devices.");
				exit(-1);
			}
		}
	}
	virtual LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		assert("WindowContainer::MessageHandler must be overrideen" && 0); return 0;
	}

	virtual bool Update()
	{
		assert("WindowContainer::Update must be overridden." && 0); return false;
	}
	virtual void RenderFrame()
	{
		assert("WindowContainer::RenderFrame must be overridden." && 0);
	}

	Window* GetWindow() const { return window.get(); }
	Timer* GetTimer() const { return timer.get(); }
};

