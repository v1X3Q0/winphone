#pragma once
#include <string>
#include <vector>
char* parseInput(wchar_t* argv, std::string *cmd, std::vector<std::string> *args);
int isValidCommand(std::string cmd);
void execCommand(std::string *cmd, std::vector<std::string> *argv);