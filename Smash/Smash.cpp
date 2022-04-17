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

Smash& Smash::Instance()
{
	static Smash instance;
	return instance;
}

Command* Smash::CreateCommand(string& cmdStr, vector<string>& cmdArgs)
{
	try
	{
		Commands cmd = CommandType(cmdArgs);

		switch (cmd)
		{
			case (Commands::Unknown): return ExternalCommand::Create(cmdStr, cmdArgs);

			case (Commands::SleepPrint): return SleepPrintCommand::Create(cmdStr, cmdArgs);

			case (Commands::Jobs): return JobsCommand::Create(cmdStr, cmdArgs);

			case (Commands::Kill): return KillCommand::Create(cmdStr, cmdArgs);

			default: return nullptr;
		}
	}
	catch (...)
	{
		return nullptr;
	}
}

string Smash::Prompt()
{
	return promptText + "> ";
}

void Smash::ExecuteCommand(string& cmdStr)
{
	bool inBackground = IsRunInBackground(cmdStr);

	vector<string> cmdArgs = ParseCommand(cmdStr);

	Command* cmd = CreateCommand(cmdStr, cmdArgs);

	jobs.RemoveFinished();

	if (cmd == nullptr)
	{
		return;
	}

	if (CommandType(cmdArgs) == Commands::Unknown)
	{
		pid_t pid = fork();

		if (pid < 0)
		{
			perror("smash error: fork failed");
		}
		else if (pid == 0)
		{
			setpgrp();
			cmd->Execute();
		}
		else if (inBackground)
		{
			jobs.AddJob(pid, cmd, false);
			return;
		}

		waitpid(pid, nullptr, 0);
		delete cmd;
		return;
	}

	cmd->Execute();
	delete cmd;
}