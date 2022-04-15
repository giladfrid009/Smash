#ifndef PARSER_H
#define PARSER_H

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

#include <string.h>
#include <iostream>

std::string LeftTrim(const std::string& str);
std::string RightTrim(const std::string& str);
std::string Trim(const std::string& str);
int ParseCommand(const char* cmdStr, char** args);
bool IsBackgroundComamnd(const char* cmdStr);
void RemoveBackgroundSign(char* cmdStr);

#endif