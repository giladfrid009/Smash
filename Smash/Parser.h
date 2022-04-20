#ifndef PARSER_H
#define PARSER_H

#include "Identifiers.h"
#include <string>
#include <vector>

std::string LeftTrim(const std::string& str);

std::string RightTrim(const std::string& str);

std::string Trim(const std::string& str);

Commands CommandType(const std::vector<std::string>& cmdArgs);

std::vector<std::string> ParseCommand(const std::string& cmdStr);

std::vector<std::string> Split(const std::string& cmdStr, std::string seperator);

bool IsRunInBackground(const std::string& cmdStr);

std::string RemoveBackgroundSign(const std::string& cmdStr);

#endif