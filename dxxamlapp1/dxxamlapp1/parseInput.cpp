#include "pch.h"
#include <string>
#include <sstream>
#include <Windows.h>
#include <cctype>
#include <algorithm>
#include <vector>
#include "enum_directory.h"

std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty())
		return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

std::string to_lower(std::string caseIns)
{
	char* data = (char*)malloc(caseIns.size());
	memcpy(data, caseIns.data(), caseIns.size());
	for (int i = 0; i = caseIns.size(); i++)
		data[i] = (char)tolower(data[i]);
	std::string new1(data);
	return new1;
}

bool BothAreSpaces(char lhs, char rhs)
{
	return (lhs == rhs) && (lhs == ' ');
}

int parseInput(wchar_t* argv, std::string *cmd, std::vector<std::string> *args)
{
	std::wstring inputStr = std::wstring(argv);
	std::string ansInpt = utf8_encode(inputStr);
	ansInpt = to_lower(ansInpt);

	auto new_end = std::unique(ansInpt.begin(), ansInpt.end(), BothAreSpaces);
	ansInpt.erase(new_end, ansInpt.end());

	std::istringstream iss(ansInpt);
	auto spacTot = std::count(ansInpt.begin(), ansInpt.end(), " ");
	
	//if (spacTot == 0)
	std::string buf;
	iss >> buf;
	*cmd = buf;
	
	for (int i = 0; i < spacTot; i++)
	{
		iss >> buf;
		args->push_back(buf);
	}
}

int isValidCommand(std::string cmd)
{
	if (cmd == "ls")
		return true;
	else if (cmd == "cd")
		return true;
	else
	{
		if (PathExi)
	}
}

void execCommand(std::string *cmd, std::vector<std::string> *argv)
{
	std::string buf = *cmd;
	if (buf == "ls")
	{
		wchar_t dir[0x100];
		GetCurrentDirectory(0x100, dir);
		enum_directory(dir);
	}
	//else
	//{
	//	CreateProcess
	//}
}
