#include "StringHelper.h"

std::wstring StringHelper::StringToWideString(std::string str)
{
	std::wstring wstr(str.begin(), str.end());
	return wstr;
}

std::string StringHelper::GetDirectoryFromPath(const std::string& filePath)
{
	size_t offset1 = filePath.find_last_of('\\');//find_last_of() : 해당 문자의 위치를 찾지 못하면 std::string::npos를 반환.
	size_t offset2 = filePath.find_last_of('/');//문자를 찾으면 배영살의 인덱스를 반환.
	if ((std::string::npos == offset1) && (std::string::npos == offset2)) return "";
	if (std::string::npos == offset1) return std::string(filePath.substr(0, offset2));
	if (std::string::npos == offset2) return std::string(filePath.substr(0, offset1));
	//두 offset값이 다 존재할 경우, 더 큰값을 사용해야 한다.
	return std::string(filePath.substr(0, std::max(offset1, offset2)));
}

std::string StringHelper::GetFileExtention(const std::string& fileName)
{
	size_t offset = fileName.find_last_of('.');
	if (std::string::npos == offset) return "";
	return std::string(fileName.substr(offset + 1));
}