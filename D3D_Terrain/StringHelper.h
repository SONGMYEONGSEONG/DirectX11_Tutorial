#pragma once
#include <string>

class StringHelper
{
public:
	static std::wstring StringToWideString(std::string str);//���ڿ��� ���̵� ���ڿ��� ��ȯ.
	static std::string GetDirectoryFromPath(const std::string& filePath);//���ϸ��� ������ ���������� ��θ� �˾ƿ´�.
	static std::string GetFileExtention(const std::string& fileName);//������ Ȯ���ڸ� �˾ƿ´�.
};

