#pragma once
//에러 및 디버깅 정보 출력을 쉽게 처리하도록 한다.
#include <Windows.h>
#include "COMException.h"

class Log
{
public:
	static void ErrorMsg(std::string msg);
	static void ErrorMsg(HRESULT hr, std::string msg);
	static void ErrorMsg(HRESULT hr, std::wstring msg);
	static void ErrorMsg(COMException& ex);

	static void Output(LPCSTR str);
	static void Output(LPCWSTR str);  
};

