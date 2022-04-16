#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include <unistd.h>
#include <string>
#include <iostream>

SleepPrintCommand::SleepPrintCommand(int duration, std::string messege) : InternalCommand()
{
	this->duration = (unsigned int)duration;
	this->messege = messege;
}

Command* SleepPrintCommand::Create(std::vector<std::string>& cmdArgs)
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
