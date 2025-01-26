//COM (Component Object Model)
//DirectX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술

#pragma once
#include <comdef.h>
#include "StringHelper.h"

class COMException
{
private:
	std::wstring error_msg;

public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		error_msg = L"Msg: " + StringHelper::StringToWideString(msg) + L"\n";
		error_msg += error.ErrorMessage();
		error_msg += L"\nFile: " + StringHelper::StringToWideString(file);
		error_msg += L"\nFunction: " + StringHelper::StringToWideString(function);
		error_msg += L"\nLine: " + std::to_wstring(line);
	}
	const wchar_t* what() const { return error_msg.c_str(); }
};
#define COM_ERROR_IF_FAILED(hr, msg) if(FAILED(hr)) throw COMException(hr, msg, __FILE__, __FUNCTION__, __LINE__)

