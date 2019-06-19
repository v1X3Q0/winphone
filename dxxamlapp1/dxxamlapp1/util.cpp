#include "pch.h"
#include <string>
#include <util.h>

using namespace Platform;
using namespace Windows::Storage;

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

char* utf8_encode(const wchar_t* wstr)
{
	int size_needed = wcslen(wstr);
	if (size_needed == 0)
		return NULL;
	char* newStr = (char*)calloc(size_needed + 1, 1);
	int result = WideCharToMultiByte(CP_UTF8, 0, wstr, size_needed, newStr, size_needed, NULL, NULL);
	if (result == 0)
	{
		result = GetLastError();
		free(newStr);
		return NULL;
	}
	return newStr;
}

wchar_t* utf16_encode(const char* str)
{
	int size_needed = strlen(str);
	if (size_needed == 0)
		return NULL;
	wchar_t* unistring = (wchar_t*)malloc((size_needed + 1) * 2);
	int result = MultiByteToWideChar(CP_OEMCP, 0, str, -1, unistring, (size_needed + 1) * 2);
	if (result == 0)
	{
		free(unistring);
		return NULL;
	}
	return unistring;
}

void resetStream(FILE** stdout_stream)
{
	String^ localfolder = ApplicationData::Current->LocalFolder->Path;
	std::wstring basePath(localfolder->Data());
	std::wstring fileName(L"\\stdout_stream.txt");
	basePath = basePath + fileName;
	const wchar_t* fullpath = basePath.c_str();
	size_t size = wcslen(fullpath) * 2 + 2;
	char * StartPoint = new char[size];
	size_t c_size;
	wcstombs_s(&c_size, StartPoint, size, fullpath, size);
	errno_t err;

	err = freopen_s(stdout_stream, StartPoint,
		"w+", stdout);
	delete[] StartPoint;

	//if (err == 0)
	//	ijk = 0;
	//else
	//	ijk = 1;
}
