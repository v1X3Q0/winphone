#include "pch.h"
#include <string>
#include <util.h>

std::wstring StringToWString(const std::string& s)
{
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}


std::string WStringToString(const std::wstring& s)
{
	std::string temp(s.length(), ' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty())
		return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

std::wstring utf16_encode(const std::string &str)
{
	if (str.empty())
		return std::wstring();
	size_t len = str.size();
	WCHAR* unistring = (WCHAR*)malloc(len + 1);
	int result = MultiByteToWideChar(CP_OEMCP, 0, str.data(), -1, unistring, len + 1);
	std::wstring strTo;
	if (result != 0)
		strTo = std::wstring(unistring);
	delete[] unistring;
	return strTo;
}
