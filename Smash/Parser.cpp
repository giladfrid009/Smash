#include "Parser.h"
#include "Identifiers.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using std::string;
using std::vector;
using std::istringstream;

const Identifiers I;

string LeftTrim(const string& cmdStr)
{
	size_t start = cmdStr.find_first_not_of(I.Whitespace);
	return (start == std::string::npos) ? "" : cmdStr.substr(start);
}

string RightTrim(const string& cmdStr)
{
	size_t end = cmdStr.find_last_not_of(I.Whitespace);
	return (end == std::string::npos) ? "" : cmdStr.substr(0, end + 1);
}

string Trim(const string& cmdStr)
{
	return RightTrim(LeftTrim(cmdStr));
}

vector<string> ParseCommand(const string& cmdStr)
{
	string formatted = Trim(RemoveBackgroundSign(cmdStr));

	istringstream iss(formatted);
	string buffer;
	vector<string> result;

	while (iss >> buffer)
	{
		result.push_back(buffer);
	}

	return result;
}

//todo: remove if unused
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

bool IsRunInBackground(const string& cmdStr)
{
	return cmdStr[cmdStr.find_last_not_of(I.Whitespace)] == '&';
}

string RemoveBackgroundSign(const string& cmdStr)
{
	size_t len = cmdStr.find_last_not_of(I.Whitespace);

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

static bool Contains(const vector<string>& cmdArgs, string predicate)
{
	return std::any_of(cmdArgs.begin(), cmdArgs.end(), [predicate] (string str) {return str == predicate; });
}

//todo: find better name
Commands CommandType(const vector<string>& cmdArgs)
{
	if (cmdArgs.size() < 1) return Commands::Unknown;

	if (Contains(cmdArgs, I.PipeOut)) return Commands::PipeOut;

	if (Contains(cmdArgs, I.PipeErr)) return Commands::PipeErr;

	if (Contains(cmdArgs, I.RedirectWrite)) return Commands::RedirectWrite;

	if (Contains(cmdArgs, I.RedirectAppend)) return Commands::RedirectAppend;

	string cmd = cmdArgs[0];

	if (cmd == I.Background) return Commands::Background;

	if (cmd == I.ChangeDir) return Commands::ChangeDir;

	if (cmd == I.ChangePrompt) return Commands::ChangePrompt;

	if (cmd == I.Foreground) return Commands::Foreground;

	if (cmd == I.Jobs) return Commands::Jobs;

	if (cmd == I.Kill) return Commands::Kill;

	if (cmd == I.PrintDir) return Commands::PrintDir;

	if (cmd == I.Quit) return Commands::Quit;

	if (cmd == I.ShowPid) return Commands::ShowPid;

	if (cmd == I.Tail) return Commands::Tail;

	if (cmd == I.Timeout) return Commands::Timeout;

	if (cmd == I.Touch) return Commands::Touch;

	return Commands::Unknown;
}