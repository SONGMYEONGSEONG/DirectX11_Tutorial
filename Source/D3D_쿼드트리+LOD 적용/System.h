#pragma once
#include "WindowContainer.h"
class System final : public WindowContainer
{
public:
	//is_base_of : Base�κ��� ��ӵ� class�� �´��� Ȯ���ϴ� �ڵ� ex) Graphics(�θ�)�� ��ӵ� T(�ڽ� Ÿ��)�ΰ�� True ����
	//enable_if : ���� �ڵ� Ȯ�� 
	/*
	template <typename T>
	struct enable_if<true, T> {
		typedef T type; // ���̸� �ش� Ŭ������ T�� �ʱ�ȭ 
	};
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Graphics, T>::value>>// DirectX11 �ʱ�ȭ �۾� -  �߰�

	bool Intialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height, bool fullScreen = false)
	{
		window = std::make_unique<Window>();
		if (!window->Intialize(this, hInstance, title, width, height, fullScreen)) return false;

		Input::Initialize(window->GetScreenWidth(), window->GetScreenHeight());
		timer = std::make_unique<Timer>();
		timer->Start();

		// DirectX11 �ʱ�ȭ �۾� -  �߰�
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

