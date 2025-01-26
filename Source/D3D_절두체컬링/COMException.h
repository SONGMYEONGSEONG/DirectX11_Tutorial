//COM (Component Object Model)
//DirectX�� ���α׷��� ��� �������� ���� ȣȯ���� �����ϰ� �ϴ� ���

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

