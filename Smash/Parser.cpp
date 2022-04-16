#include "Parser.h"
#include "Identifiers.h"
#include <string>
#include <sstream>
#include <vector>

using std::string;
using std::vector;
using std::istringstream;

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

vector<string> ParseCommand(const string& cmdStr)
{
	istringstream iss(Trim(cmdStr));
	string buffer;
	vector<string> result;

	while (iss >> buffer)
	{
		result.push_back(buffer);
	}

	return result;
}

vector<string> Split(const string& cmdStr, string seperator)
{
	size_t sepLen = seperator.length();
	size_t startPos = 0;
	size_t endPos;
	string buffer;
	vector<string> result;

	while ((endPos = cmdStr.find(seperator, startPos)) != string::npos)
	{
		buffer = cmdStr.substr(startPos, endPos - startPos);
		startPos = endPos + sepLen;
		result.push_back(buffer);
	}

	result.push_back(cmdStr.substr(startPos));

	return result;
}

Commands GetCommand(const char* cmdStr)
{
	Identifiers I;
	const string str(cmdStr);

	if (GetSpecialCommand(cmdStr) != SpecialCommands::None)
	{
		return Commands::Special;
	}

	if (str.find(I.Background) != string::npos)
	{
		return Commands::Background;
	}

	if (str.find(I.ChangeDir) != string::npos)
	{
		return Commands::ChangeDir;
	}

	if (str.find(I.ChangePrompt) != string::npos)
	{
		return Commands::ChangePrompt;
	}

	if (str.find(I.Foreground) != string::npos)
	{
		return Commands::Foreground;
	}

	if (str.find(I.Jobs) != string::npos)
	{
		return Commands::Jobs;
	}

	if (str.find(I.Kill) != string::npos)
	{
		return Commands::Kill;
	}

	if (str.find(I.Quit) != string::npos)
	{
		return Commands::Quit;
	}

	if (str.find(I.ShowPid) != string::npos)
	{
		return Commands::ShowPid;
	}

	if (str.find(I.Tail) != string::npos)
	{
		return Commands::Tail;
	}

	if (str.find(I.Touch) != string::npos)
	{
		return Commands::Touch;
	}

	return Commands::Unknown;
}

SpecialCommands GetSpecialCommand(const char* cmdStr)
{
	Identifiers I;
	const string str(cmdStr);

	if (str.find(I.Timeout) != string::npos)
	{
		return SpecialCommands::Timeout;
	}

	if (str.find(I.PipeErr) != string::npos)
	{
		return SpecialCommands::PipeErr;
	}

	if (str.find(I.PipeOut) != string::npos)
	{
		return SpecialCommands::PipeOut;
	}

	if (str.find(I.RedirectAppend) != string::npos)
	{
		return SpecialCommands::RedirectAppend;
	}

	if (str.find(I.RedirectWrite) != string::npos)
	{
		return SpecialCommands::RedirectWrite;
	}

	return SpecialCommands::None;
}

bool IsRunInBackground(const string& cmdStr)
{
	return cmdStr[cmdStr.find_last_not_of(WHITESPACE)] == '&';
}

string RemoveBackgroundSign(const string& cmdStr)
{
	size_t len = cmdStr.find_last_not_of(WHITESPACE);

	if (len == string::npos)
	{
		return cmdStr;
	}

	if (cmdStr[len] != '&')
	{
		return cmdStr;
	}

	return RightTrim(cmdStr.substr(0, len));
}
