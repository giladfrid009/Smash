#include "Parser.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

const string WHITESPACE = " \n\r\t\f\v";

string LeftTrim(const string& str)
{
	size_t start = str.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : str.substr(start);
}

string RightTrim(const string& str)
{
	size_t end = str.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

string Trim(const string& str)
{
	return RightTrim(LeftTrim(str));
}

int ParseCommand(const char* cmdStr, char** args)
{
	FUNC_ENTRY()
		int i = 0;

	std::istringstream iss(Trim(std::string(cmdStr)).c_str());

	for (string s; iss >> s; )
	{
		args[i] = (char*)malloc(s.length() + 1);
		memset(args[i], 0, s.length() + 1);
		strcpy(args[i], s.c_str());
		args[++i] = NULL;
	}
	return i;

	FUNC_EXIT()
}

bool IsBackgroundComamnd(const char* cmdStr)
{
	const string str(cmdStr);
	return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void RemoveBackgroundSign(char* cmdStr)
{
	const string str(cmdStr);
	size_t idx = str.find_last_not_of(WHITESPACE);

	if (idx == string::npos)
	{
		return;
	}

	if (cmdStr[idx] != '&')
	{
		return;
	}
	cmdStr[idx] = ' ';
	cmdStr[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}
