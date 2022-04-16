#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include <unistd.h>
#include <string>
#include <iostream>

using std::string;
using std::vector;

Command::Command(std::string cmdStr)
{
	this->cmdStr = cmdStr;
}

std::string Command::GetCommandString()
{
	return cmdStr;
}

ExternalCommand::ExternalCommand(string& cmdStr) : Command(cmdStr)
{
}

Command* ExternalCommand::Create(string& cmdStr, std::vector<std::string>& cmdArgs)
{
	try
	{
		return new ExternalCommand(cmdStr);
	}
	catch (...)
	{
		return nullptr;
	}
}

void ExternalCommand::Execute()
{
	string formattedCmd = "bash -c \"" + cmdStr + "\"";
	system(formattedCmd.c_str());
}

SleepPrintCommand::SleepPrintCommand(std::string& cmdStr, int duration, string messege) : InternalCommand(cmdStr)
{
	this->duration = (unsigned int)duration;
	this->messege = messege;
}

Command* SleepPrintCommand::Create(std::string& cmdStr, std::vector<std::string>& cmdArgs)
{
	try
	{
		if (cmdArgs.size() != 3)
		{
			return nullptr;
		}

		if (GetCommand(cmdArgs[0]) != Commands::SleepPrint)
		{
			return nullptr;
		}

		int duration = std::stoi(cmdArgs[1]);

		return new SleepPrintCommand(cmdStr, duration, cmdArgs[2]);
	}
	catch (...)
	{
		return nullptr;
	}
}

void SleepPrintCommand::Execute()
{
	sleep(duration);
	std::cout << messege << "\n";
}




