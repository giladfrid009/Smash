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

std::string Command::CommandString()
{
	return cmdStr;
}

ExternalCommand::ExternalCommand(string& cmdStr) : Command(cmdStr)
{
}

Command* ExternalCommand::Create(string& cmdStr, vector<string>& cmdArgs)
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
	string formatted = Trim(RemoveBackgroundSign(cmdStr));

	char* args[] = {"bash", "-c", &formatted[0], NULL};

	execv("/bin/bash", args);

	perror("smash error: execv failure");

	exit(0);
}

SleepPrintCommand::SleepPrintCommand(string& cmdStr, int duration, string messege) : InternalCommand(cmdStr)
{
	this->duration = (unsigned int)duration;
	this->messege = messege;
}

Command* SleepPrintCommand::Create(string& cmdStr, vector<string>& cmdArgs)
{
	try
	{
		if (CommandType(cmdArgs) != Commands::SleepPrint)
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
