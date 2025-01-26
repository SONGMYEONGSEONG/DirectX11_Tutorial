#pragma once
#define WIN32_LEAN_ANE_MEAN // 자주 사용되지 않는 WinAPI 코드를 제외
#include <Windows.h>
#include "Log.h"
struct WINDOW_DESC
{
	LPCWSTR title;
	int x, y;
	UINT width, height;
	DWORD dwStyle;
};

class WindowContainer;
class Window
{
private:
	HINSTANCE hInstance;
	HWND hWnd;
	WINDOW_DESC windowDesc;
	bool fullScreen;
	int screenWidth, screenHeight;
	WindowContainer* pContainer;

public:
	Window();
	~Window();

	bool Intialize(WindowContainer* pContainer, HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height,
		bool fullScreen = false);
	void Processing();

	HWND GetHWND() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	bool IsFullScreen() const;

private:
	void DescriptionWindow(LPCWSTR title, UINT width, UINT height);
	void RegisterWindowClass();
	bool CreateAndShowWindow(WindowContainer* pContainer);
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wapram, LPARAM lparam);


