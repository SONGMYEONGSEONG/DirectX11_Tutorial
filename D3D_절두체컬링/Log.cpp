#include "Log.h"

void Log::ErrorMsg(std::string msg)
{
	MessageBoxA(NULL, msg.c_str(), "Error", MB_ICONERROR);
}

void Log::ErrorMsg(HRESULT hr, std::string msg)
{
	ErrorMsg(hr,StringHelper::StringToWideString(msg));
}

void Log::ErrorMsg(HRESULT hr, std::wstring msg)
{
	_com_error error(hr);
	std::wstring log = msg + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, log.c_str(), L"Error", MB_ICONERROR);
}

void Log::ErrorMsg(COMException& ex)
{
	MessageBoxW(NULL, ex.what(), L"Error", MB_ICONERROR);
}

//OutputDebugString() : 문자열을 [출력]창에 출력한다.
void Log::Output(LPCSTR str) { OutputDebugStringA(str); }
void Log::Output(LPCWSTR str) { OutputDebugStringW(str); }

