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
	currentPid = -1;
	prompt = "smash";
}

Smash::~Smash()
{
}

Smash& Smash::Instance()
{
	static Smash instance;
	return instance;
}

string Smash::Prompt() const
{
	return prompt + "> ";
}

pid_t Smash::CurrentPid() const
{
	return currentPid;
}

Command* Smash::CreateCommand(string& cmdStr, vector<string>& cmdArgs) const
{
	try
	{
		Commands cmd = CommandType(cmdArgs);

		switch (cmd)
		{
			case (Commands::Unknown): return ExternalCommand::Create(cmdStr, cmdArgs);

			case (Commands::Jobs): return JobsCommand::Create(cmdStr, cmdArgs);

			case (Commands::Kill): return KillCommand::Create(cmdStr, cmdArgs);

			case (Commands::Background): return BackgroundCommand::Create(cmdStr, cmdArgs);

			case (Commands::Foreground): return ForegroundCommand::Create(cmdStr, cmdArgs);

			case (Commands::ChangePrompt): return ChangePromptCommand::Create(cmdStr, cmdArgs);

			case (Commands::ShowPid): return ShowPidCommand::Create(cmdStr, cmdArgs);

			case (Commands::Quit): return QuitCommand::Create(cmdStr, cmdArgs);

			default: return nullptr;
		}
	}
	catch (...)
	{
		return nullptr;
	}
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
			return;
		}
		else if (inBackground)
		{
			jobs.AddJob(pid, cmd, false);
			return;
		}

		int exitStat;

		currentPid = pid;

		waitpid(pid, &exitStat, WUNTRACED);

		currentPid = -1;

		if (WIFSTOPPED(exitStat))
		{
			jobs.AddJob(pid, cmd, true);
			return;
		}

		delete cmd;
		return;
	}

	cmd->Execute();
	delete cmd;
}


