#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include "Smash.h"
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <signal.h>

using std::string;
using std::vector;

static void SysError(string sysCall)
{
	string formatted = "smash error: " + sysCall + " failed";
	perror(formatted.c_str());
}

Command::Command(const std::string& cmdStr)
{
	this->cmdStr = cmdStr;
}

std::string Command::ToString()
{
	return cmdStr;
}

InternalCommand::InternalCommand(const std::string& cmdStr) : Command(cmdStr)
{
}

ExternalCommand::ExternalCommand(const string& cmdStr) : Command(cmdStr)
{
}

Command* ExternalCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	return new ExternalCommand(cmdStr);
}

void ExternalCommand::Execute()
{
	string formatted = Trim(RemoveBackgroundSign(cmdStr));

	if (formatted.empty())
	{
		formatted = " ";
	}

	char* const args[] = {"bash", "-c", &formatted[0], NULL};

	execv("/bin/bash", args);

	SysError("execv");

	exit(0);
}

JobsCommand::JobsCommand(const std::string& cmdStr) : InternalCommand(cmdStr)
{
}

Command* JobsCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
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
	instance.jobs.PrintJobs();
}

KillCommand::KillCommand(const std::string& cmdStr, int signalNum, int jobID) : InternalCommand(cmdStr)
{
	this->signalNum = signalNum;
	this->jobID = jobID;
}

Command* KillCommand::Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Kill)
	{
		return nullptr;
	}

	if (cmdArgs.size() != 3)
	{
		std::cerr << "smash error: kill: invalid arguments" << std::endl;
		return nullptr;
	}

	try
	{
		int signalNum = (-1) * std::stoi(cmdArgs[1]);
		int jobID = std::stoi(cmdArgs[2]);
		return new KillCommand(cmdStr, signalNum, jobID);
	}
	catch (...)
	{
		std::cerr << "smash error: kill: invalid arguments" << std::endl;
		return nullptr;
	}
}

void KillCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int pid = instance.jobs.GetPid(jobID);

	if (pid < 0)
	{
		std::cerr << "smash error: kill: job-id " << jobID << " does not exist" << std::endl;
		return;
	}

	int res = kill(pid, signalNum);

	if (res < 0)
	{
		SysError("kill");
	}
}

Command* BackgroundCommand::Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Background)
	{
		return nullptr;
	}

	if (cmdArgs.size() == 1)
	{
		return new BackgroundCommand(cmdStr);
	}

	if (cmdArgs.size() == 2)
	{
		try
		{
			int jobID = std::stoi(cmdArgs[1]); //todo: check ASCII validation

			return new BackgroundCommand(cmdStr, jobID);
		}
		catch (...)
		{
			std::cerr << "smash error: bg: invalid arguments" << std::endl;
		}
	}

	std::cerr << "smash error: bg: invalid arguments" << std::endl;

	return nullptr;
}

BackgroundCommand::BackgroundCommand(const std::string& cmdStr) : InternalCommand(cmdStr)
{
	jobID = -1;
	useID = false;
}

BackgroundCommand::BackgroundCommand(const std::string& cmdStr, int jobID) : InternalCommand(cmdStr)
{
	this->jobID = jobID;
	this->useID = true;
}

void BackgroundCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int dstID = -1;

	if (useID)
	{
		JobStatus status = instance.jobs.GetStatus(jobID);

		if (status == JobStatus::Finished || status == JobStatus::Unknown)
		{
			std::cerr << "smash error: bg: job-id " << jobID << " does not exist" << std::endl;
			return;
		}

		if (status == JobStatus::Running)
		{
			std::cerr << "smash error: bg: job-id " << jobID << " is already running in the background" << std::endl;
			return;
		}

		dstID = jobID;
	}
	else
	{
		dstID = instance.jobs.MaxStopped();

		if (dstID == -1)
		{
			std::cerr << "smash error: bg: there is no stopped jobs to resume" << std::endl;
			return;
		}
	}

	KillCommand killComm("", SIGCONT, dstID);

	killComm.Execute();

	instance.jobs.SetStatus(dstID, JobStatus::Running);
}

Command* ChangePromptCommand::Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::ChangePrompt)
	{
		return nullptr;
	}

	if (cmdArgs.size() == 1)
	{
		return new ChangePromptCommand(cmdStr, "smash");
	}
	else if (cmdArgs.size() > 1)
	{
		return new ChangePromptCommand(cmdStr, cmdArgs[1]);
	}

	return nullptr;
}

ChangePromptCommand::ChangePromptCommand(const std::string& cmdStr, std::string prompt) : InternalCommand(cmdStr)
{
	this->prompt = prompt;
}

void ChangePromptCommand::Execute()
{
	Smash& instatnce = Smash::Instance();

	instatnce.prompt = this->prompt;
}

Command* ShowPidCommand::Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::ShowPid)
	{
		return nullptr;
	}

	return new ShowPidCommand(cmdStr);
}

ShowPidCommand::ShowPidCommand(const std::string& cmdStr) : InternalCommand(cmdStr)
{
}

void ShowPidCommand::Execute()
{
	pid_t pid = getpid();

	if (pid < 0)
	{
		SysError("getpid");
		return;
	}

	std::cout << "smash pid is " << pid << std::endl;
}

Command* ForegroundCommand::Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Foreground)
	{
		return nullptr;
	}

	if (cmdArgs.size() == 1)
	{
		return new ForegroundCommand(cmdStr);
	}

	if (cmdArgs.size() == 2)
	{
		try
		{
			int jobID = std::stoi(cmdArgs[1]); //todo: check ASCII validation
			return new ForegroundCommand(cmdStr, jobID);
		}
		catch (...)
		{
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
		}
	}

	std::cerr << "smash error: fg: invalid arguments" << std::endl;

	return nullptr;
}

ForegroundCommand::ForegroundCommand(const std::string& cmdStr) : InternalCommand(cmdStr)
{
	jobID = -1;
	useID = false;
}

ForegroundCommand::ForegroundCommand(const std::string& cmdStr, int jobID) : InternalCommand(cmdStr)
{
	this->jobID = jobID;
	this->useID = true;
}

void ForegroundCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int dstID = -1;

	if (useID)
	{
		JobStatus status = instance.jobs.GetStatus(jobID);

		if (status == JobStatus::Finished || status == JobStatus::Unknown)
		{
			std::cerr << "smash error: fg: job-id " << jobID << " does not exist" << std::endl;
			return;
		}

		dstID = jobID;
	}
	else
	{
		dstID = instance.jobs.MaxID();

		if (dstID == -1)
		{
			std::cerr << "smash error: fg: jobs list is empty" << std::endl;
			return;
		}
	}

	if (instance.jobs.GetStatus(dstID) == JobStatus::Stopped)
	{
		KillCommand killComm("", SIGCONT, dstID);

		killComm.Execute();

		//todo: probably set status to running 
	}

	pid_t pid = instance.jobs.GetPid(dstID);

	Command* cmd = instance.jobs.Remove(dstID); //todo: probably should not remove to retain original id

	int exitStat;

	instance.currentPid = pid;

	pid = waitpid(pid, &exitStat, WUNTRACED);

	instance.currentPid = -1;

	if (pid < 0)
	{
		SysError("waitpid");
		return;
	}

	if (WIFSTOPPED(exitStat))
	{
		//todo: not re-add it but reset the timer, and set it's status to stopped
		//todo: maybe remove isStopped from AddJob
		instance.jobs.AddJob(pid, cmd, true);
	}
}

Command* QuitCommand::Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Quit)
	{
		return nullptr;
	}

	if (cmdArgs.size() >= 2 && cmdArgs[1] == "kill")
	{
		return new QuitCommand(cmdStr, true);
	}

	return new QuitCommand(cmdStr, false);
}

QuitCommand::QuitCommand(const std::string& cmdStr, bool killChildren) : InternalCommand(cmdStr)
{
	this->killChildren = killChildren;
}

void QuitCommand::Execute()
{
	if (killChildren)
	{
		Smash& instance = Smash::Instance();

		std::cout << "smash: sending SIGKILL signal to " << instance.jobs.Size() << " jobs:" << std::endl;

		instance.jobs.PrintQuit();

		instance.jobs.KillAll();
	}

	exit(0);
}
