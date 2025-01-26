#include "StringHelper.h"

std::wstring StringHelper::StringToWideString(std::string str)
{
	std::wstring wstr(str.begin(), str.end());
	return wstr;
}

std::string StringHelper::GetDirectoryFromPath(const std::string& filePath)
{
	size_t offset1 = filePath.find_last_of('\\');//find_last_of() : �ش� ������ ��ġ�� ã�� ���ϸ� std::string::npos�� ��ȯ.
	size_t offset2 = filePath.find_last_of('/');//���ڸ� ã���� �迵���� �ε����� ��ȯ.
	if ((std::string::npos == offset1) && (std::string::npos == offset2)) return "";
	if (std::string::npos == offset1) return std::string(filePath.substr(0, offset2));
	if (std::string::npos == offset2) return std::string(filePath.substr(0, offset1));
	//�� offset���� �� ������ ���, �� ū���� ����ؾ� �Ѵ�.
	return std::string(filePath.substr(0, std::max(offset1, offset2)));
}

std::string StringHelper::GetFileExtention(const std::string& fileName)
{
	size_t offset = fileName.find_last_of('.');
	if (std::string::npos == offset) return "";
	return std::string(fileName.substr(offset + 1));
}