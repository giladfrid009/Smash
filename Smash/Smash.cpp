#include "Smash.h"
#include "Commands.h"
#include "Parser.h"

#include <vector>
#include <string>
#include <unistd.h>
#include <iostream>

using std::string;
using std::vector;

Smash::Smash()
{
}

Smash::~Smash()
{
}

Command* Smash::CreateCommand(string& cmdStr)
{
	Commands cmdKind = GetCommand(cmdStr);

	vector<string> cmdArgs = ParseCommand(cmdStr);

	switch (cmdKind)
	{
		case (Commands::Unknown):
		{
			return ExternalCommand::Create(cmdStr, cmdArgs);
		}
		case (Commands::SleepPrint):
		{
			return SleepPrintCommand::Create(cmdStr, cmdArgs);
		}
		default:
		{
			return nullptr;
		}
	}

	return nullptr;
}

inline string Smash::GetPrompt()
{
	return promptText + "> ";
}

void Smash::ExecuteCommand(string& cmdStr)
{
	Command* cmd = CreateCommand(cmdStr);

	if (cmd == nullptr)
	{
		return;
	}

	if (GetCommand(cmdStr) == Commands::Unknown) //todo: need to fork and execv and not use system function
	{
		pid_t pid = fork();

		if (pid == 0)
		{
			//todo: move into cmd->Execute() probably
			setpgrp();

			char* arr[] = {"bash", "-c", &cmdStr[0], NULL};

			execv("/bin/bash", arr);
			std::cout << "cant exec\n";
			exit(0);
		}
		else if (pid > 0)
		{
			waitpid(pid, nullptr, 0);
			std::cout << "success\n";
		}
		else
		{
			std::cout << "failed fork\n";
		}
	}
	else
	{
		cmd->Execute();
	}

	delete cmd;
}