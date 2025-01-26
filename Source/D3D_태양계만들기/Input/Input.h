#pragma once
#include <memory>
// <memory> : ��ü�� �Ҵ��ϰ� �����ϴ� �� ������ �Ǵ� Ŭ����
#include "Keyboard/InputKeyboard.h"
#include "Mouse/InputMouse.h"

//��ǲ �Ŵ��� ����
class Input
{
private:
	//unique_ptr ( ����ũ ������,���� ������) <- ����Ʈ ������
	//�����͸� �ܵ����� ���� , �������� ���� ���� ,���糪 ���� �Ұ��� 
	//��ü�� �Ҹ�ɶ� (�Ҹ��ڸ� ȣ���Ҷ�) �����͵� �Ҹ��Ѵ� -> �ڵ� �Ҵ����� 
	static std::unique_ptr<InputKeyboard> keyboard;
	static std::unique_ptr<InputMouse> mouse;

public:
	static void Initialize(UINT screenWidth, UINT screenHeight = 0);

	static InputKeyboard* GetKeyboard() { return keyboard.get(); }
	static InputMouse* GetMouse() { return mouse.get(); }

	static void Update();
	static void MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};