#pragma once
#include <string>
std::wstring StringToWString(const std::string& s);
std::string WStringToString(const std::wstring& s);
char* utf8_encode(const wchar_t*);
wchar_t* utf16_encode(const char*);
void resetStream(FILE** stdout_stream);