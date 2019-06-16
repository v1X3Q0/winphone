#include "pch.h"
#include <string>
#include <sstream>
#include <Windows.h>
#include <cctype>
#include <algorithm>
#include <vector>
#include "util.h"
#include "enum_directory.h"

extern FILE* stdout_stream;

std::string to_lower(std::string caseIns)
{
	char* data = (char*)malloc(caseIns.size() + 1);
	memset(data, 0, caseIns.size() + 1);
	memcpy(data, caseIns.data(), caseIns.size());
	for (int i = 0; i < caseIns.size(); i++)
		data[i] = (char)tolower(data[i]);
	std::string new1(data);
	free(data);
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
	auto spacTot = std::count(ansInpt.begin(), ansInpt.end(), ' ');
	
	//if (spacTot == 0)
	std::string buf;
	iss >> buf;
	*cmd = buf;
	
	for (int i = 0; i < spacTot; i++)
	{
		iss >> buf;
		args->push_back(buf);
	}
	return 0;
}

int isValidCommand(std::string cmd)
{
	if (cmd == "ls")
		return true;
	else if (cmd == "cd")
		return true;
	//else
	//{
	//	if (PathExi)
	//}
}

void execCommand(std::string *cmd, std::vector<std::string> *argv)
{
	std::string buf = *cmd;
	if (buf == "ls")
	{
		wchar_t dir[0x100];
		GetCurrentDirectory(0x100, dir);
		char dirC[0x200];
		memset(dirC, 0, 0x200);
		std::wstring wstrTo(dir);
		std::string strTo = utf8_encode(dir);
		memcpy(dirC, strTo.data(), strTo.size());
		enum_directory(dirC);
	}
	else if (buf == "echo")
	{
		fprintf(stdout, "successfully reassigned\n");
		//printf("%s ", buf);
		//for (int i = 0; i < argv->size(); i++)
		//	printf("%s ", (*argv)[i]);
		//printf("\n");
	}
	//else
	//{
	//	CreateProcess
	//}
}
