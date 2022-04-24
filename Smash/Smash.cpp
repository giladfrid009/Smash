#include "Smash.h"
#include "Commands.h"
#include "Special.h"
#include "Parser.h"
#include <vector>
#include <string>
#include <unistd.h>
#include <iostream>

using std::string;
using std::vector;

Smash::Smash()
{
	runningPID = -1;
	prompt = "smash";
	prevPath = "";

	selfPID = getpid();

	if (selfPID < 0)
	{
		perror("smash error: getpid failed");
	}
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

pid_t Smash::RunningPID() const
{
	return runningPID;
}

pid_t Smash::SelfPID() const
{
	return selfPID;
}

Command* Smash::CreateCommand(const string& cmdStr, const vector<string>& cmdArgs) const
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

			case (Commands::ShowPID): return ShowPIDCommand::Create(cmdStr, cmdArgs);

			case (Commands::Quit): return QuitCommand::Create(cmdStr, cmdArgs);

			case (Commands::RedirectWrite): return RedirectWriteCommand::Create(cmdStr, cmdArgs);

			case (Commands::RedirectAppend): return RedirectAppendCommand::Create(cmdStr, cmdArgs);

			case (Commands::PipeOut): return PipeOutCommand::Create(cmdStr, cmdArgs);

			case (Commands::PipeErr): return PipeErrCommand::Create(cmdStr, cmdArgs);

			case (Commands::PrintDir): return PrintDirCommand::Create(cmdStr, cmdArgs);

			case (Commands::ChangeDir): return ChangeDirCommand::Create(cmdStr, cmdArgs);

			case (Commands::Touch): return TouchCommand::Create(cmdStr, cmdArgs);

			case (Commands::Tail): return TailCommand::Create(cmdStr, cmdArgs);

			case (Commands::Timeout): return nullptr;

			default: return nullptr;
		}
	}
	catch (...)
	{
		return nullptr;
	}
}

void Smash::Execute(const string& cmdStr)
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
			return;
		}

		if (pid == 0)
		{
			if (setpgrp() < 0)
			{
				perror("smash error: setpgrp failed");
				exit(1);
			}

			cmd->Execute();

			exit(0);
		}

		if (inBackground)
		{
			jobs.Add(pid, cmd);
			return;
		}

		int exitStat;

		runningPID = pid;

		waitpid(pid, &exitStat, WUNTRACED);

		runningPID = -1;

		if (WIFSTOPPED(exitStat))
		{
			jobs.Add(pid, cmd, JobStatus::Stopped);
			return;
		}

		delete cmd;
		return;
	}

	cmd->Execute();
	delete cmd;
}
