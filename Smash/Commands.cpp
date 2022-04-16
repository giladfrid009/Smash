#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include <unistd.h>
#include <string>
#include <iostream>

using std::string;
using std::vector;

SleepPrintCommand::SleepPrintCommand(int duration, string messege) : InternalCommand()
{
	this->duration = (unsigned int)duration;
	this->messege = messege;
}

Command* SleepPrintCommand::Create(vector<string>& cmdArgs)
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

		return new SleepPrintCommand(duration, cmdArgs[2]);
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

ExternalCommand::ExternalCommand(string& cmdStr) : Command()
{
	this->cmdStr = cmdStr;
}

Command* ExternalCommand::Create(string& cmdStr)
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
