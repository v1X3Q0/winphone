#pragma once
#include <string>
std::wstring StringToWString(const std::string& s);
std::string WStringToString(const std::wstring& s);
std::string utf8_encode(const std::wstring &wstr);
std::wstring utf16_encode(const std::string &str);
