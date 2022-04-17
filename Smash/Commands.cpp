#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include "Smash.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <signal.h>

using std::string;
using std::vector;

void SysError(string sysCall)
{
	string formatted = "smash error: " + sysCall + " failed";
	perror(formatted.c_str());
}

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

	SysError("execv");

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

JobsCommand::JobsCommand(std::string& cmdStr) : InternalCommand(cmdStr)
{
}

Command* JobsCommand::Create(string& cmdStr, vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Jobs)
	{
		return nullptr;
	}

	return new JobsCommand(cmdStr);
}

void JobsCommand::Execute()
{
	Smash& instance = Smash::Instance();
	instance.jobs.Print();
}

KillCommand::KillCommand(std::string& cmdStr, int signalNum, int jobId) : InternalCommand(cmdStr)
{
	this->signalNum = signalNum;
	this->jobId = jobId;
}

Command* KillCommand::Create(std::string& cmdStr, std::vector<std::string>& cmdArgs)
{
	if (cmdArgs.size() != 3)
	{
		fprintf(stderr, "smash error: kill: invalid arguments\n");
		return nullptr;
	}

	if (CommandType(cmdArgs) != Commands::Kill)
	{
		return nullptr;
	}

	try
	{
		int signalNum = (-1) * std::stoi(cmdArgs[1]);
		int jobId = std::stoi(cmdArgs[2]);
		return new KillCommand(cmdStr, signalNum, jobId);
	}
	catch (...)
	{
		fprintf(stderr, "smash error: kill: invalid arguments\n");
		return nullptr;
	}
}

void KillCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int pid = instance.jobs.GetPid(jobId);

	//todo: go over all error handling
	//todo: make all prints this way

	if (pid < 0)
	{
		fprintf(stderr, "smash error: kill: job-id {0} does not exist\n", jobId);
		return;
	}

	int res = kill(pid, signalNum);

	if (res < 0)
	{
		SysError("kill");
	}
}
