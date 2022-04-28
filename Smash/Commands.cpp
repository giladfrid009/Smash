#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include "Smash.h"
#include <string>
#include <vector>
#include <signal.h>
#include <exception>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

static void SysError(string sysCall)
{
	string formatted = "smash error: " + sysCall + " failed";
	perror(formatted.c_str());
}

Command::Command(const string& cmdStr)
{
	this->cmdStr = cmdStr;
}

string Command::ToString()
{
	return cmdStr;
}

InternalCommand::InternalCommand(const string& cmdStr) : Command(cmdStr)
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

	char* const args[] = {"/bin/bash", "-c", &formatted[0], NULL};

	execv("/bin/bash", args);

	SysError("execv");

	exit(1);
}

JobsCommand::JobsCommand(const string& cmdStr) : InternalCommand(cmdStr)
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

	instance.jobs.ForEach([] (const JobEntry& job) { job.PrintJob(); });
}

KillCommand::KillCommand(const string& cmdStr, int signalNum, int jobID) : InternalCommand(cmdStr)
{
	if (signalNum <= 0 || signalNum >= 32)
	{
		throw std::invalid_argument("signalNum");
	}

	this->signalNum = signalNum;
	this->jobID = jobID;
}

Command* KillCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Kill)
	{
		return nullptr;
	}

	if (cmdArgs.size() != 3)
	{
		cerr << "smash error: kill: invalid arguments" << endl;
		return nullptr;
	}

	try
	{
		if (cmdArgs[1][0] != '-')
		{
			cerr << "smash error: kill: invalid arguments" << endl;
			return nullptr;
		}

		int signalNum = (-1) * std::stoi(cmdArgs[1]);
		int jobID = std::stoi(cmdArgs[2]);

		return new KillCommand(cmdStr, signalNum, jobID);
	}
	catch (...)
	{
		cerr << "smash error: kill: invalid arguments" << endl;
		return nullptr;
	}
}

void KillCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int pid = instance.jobs.GetPID(jobID);

	if (pid < 0)
	{
		cerr << "smash error: kill: job-id " << jobID << " does not exist" << endl;
		return;
	}

	int res = kill(pid, signalNum);

	if (res < 0)
	{
		SysError("kill"); return;
	}

	cout << "signal number " << signalNum << " was sent to pid " << pid << endl;
}

Command* BackgroundCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
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
			int jobID = std::stoi(cmdArgs[1]);

			return new BackgroundCommand(cmdStr, jobID);
		}
		catch (...)
		{
			cerr << "smash error: bg: invalid arguments" << endl;
			return nullptr;
		}
	}

	cerr << "smash error: bg: invalid arguments" << endl;

	return nullptr;
}

BackgroundCommand::BackgroundCommand(const string& cmdStr) : InternalCommand(cmdStr)
{
	jobID = -1;
	useID = false;
}

BackgroundCommand::BackgroundCommand(const string& cmdStr, int jobID) : InternalCommand(cmdStr)
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
			cerr << "smash error: bg: job-id " << jobID << " does not exist" << endl;
			return;
		}

		if (status == JobStatus::Running)
		{
			cerr << "smash error: bg: job-id " << jobID << " is already running in the background" << endl;
			return;
		}

		dstID = jobID;
	}
	else
	{
		dstID = instance.jobs.MaxStopped();

		if (dstID == -1)
		{
			cerr << "smash error: bg: there is no stopped jobs to resume" << endl;
			return;
		}
	}

	pid_t pid = instance.jobs.GetPID(dstID);

	instance.jobs.PrintCommand(dstID);

	cout << " : " << pid << endl;

	int res = kill(pid, SIGCONT);

	if (res < 0)
	{
		SysError("kill"); return;
	}

	instance.jobs.SetStatus(dstID, JobStatus::Running);
}

Command* ChangePromptCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
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

ChangePromptCommand::ChangePromptCommand(const string& cmdStr, string prompt) : InternalCommand(cmdStr)
{
	this->prompt = prompt;
}

void ChangePromptCommand::Execute()
{
	Smash& instatnce = Smash::Instance();

	instatnce.prompt = this->prompt;
}

Command* ShowPIDCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::ShowPID)
	{
		return nullptr;
	}

	return new ShowPIDCommand(cmdStr);
}

ShowPIDCommand::ShowPIDCommand(const string& cmdStr) : InternalCommand(cmdStr)
{
}

void ShowPIDCommand::Execute()
{
	cout << "smash pid is " << Smash::Instance().SelfPID() << endl;
}

Command* ForegroundCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
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
			int jobID = std::stoi(cmdArgs[1]);

			return new ForegroundCommand(cmdStr, jobID);
		}
		catch (...)
		{
			cerr << "smash error: fg: invalid arguments" << endl;
			return nullptr;
		}
	}

	cerr << "smash error: fg: invalid arguments" << endl;

	return nullptr;
}

ForegroundCommand::ForegroundCommand(const string& cmdStr) : InternalCommand(cmdStr)
{
	jobID = -1;
	useID = false;
}

ForegroundCommand::ForegroundCommand(const string& cmdStr, int jobID) : InternalCommand(cmdStr)
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
			cerr << "smash error: fg: job-id " << jobID << " does not exist" << endl;
			return;
		}

		dstID = jobID;
	}
	else
	{
		dstID = instance.jobs.MaxID();

		if (dstID == -1)
		{
			cerr << "smash error: fg: jobs list is empty" << endl;
			return;
		}
	}

	pid_t pid = instance.jobs.GetPID(dstID);

	if (instance.jobs.GetStatus(dstID) == JobStatus::Stopped)
	{
		int res = kill(pid, SIGCONT);

		if (res < 0)
		{
			SysError("kill"); return;
		}

		instance.jobs.SetStatus(dstID, JobStatus::Running);
	}


	instance.jobs.PrintCommand(dstID);

	cout << " : " << pid << endl;

	int exitStat;

	instance.runningPID = pid;

	pid = waitpid(pid, &exitStat, WUNTRACED);

	instance.runningPID = -1;

	if (pid < 0)
	{
		SysError("waitpid");
		return;
	}

	if (WIFSTOPPED(exitStat))
	{
		instance.jobs.SetStatus(dstID, JobStatus::Stopped);
		instance.jobs.ResetTime(dstID);
	}
	else
	{
		instance.jobs.SetStatus(dstID, JobStatus::Finished);
	}
}

Command* QuitCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
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

QuitCommand::QuitCommand(const string& cmdStr, bool killChildren) : InternalCommand(cmdStr)
{
	this->killChildren = killChildren;
}

void QuitCommand::Execute()
{
	if (killChildren)
	{
		Smash& instance = Smash::Instance();

		cout << "smash: sending SIGKILL signal to " << instance.jobs.Size() << " jobs:" << endl;

		instance.jobs.ForEach([] (const JobEntry& job) { job.PrintQuit(); });

		instance.jobs.ForEach([] (const JobEntry& job) { int res = kill(job.PID(), SIGKILL); if (res < 0) SysError("kill"); });
	}

	exit(0);
}

Command* PrintDirCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::PrintDir)
	{
		return nullptr;
	}

	return new PrintDirCommand(cmdStr);
}

PrintDirCommand::PrintDirCommand(const string& cmdStr) : InternalCommand(cmdStr)
{
}

void PrintDirCommand::Execute()
{
	char* path = get_current_dir_name();

	if (path == nullptr)
	{
		SysError("getcwd");
		return;
	}

	cout << path << endl;

	free(path);
}

Command* ChangeDirCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::ChangeDir)
	{
		return nullptr;
	}

	if (cmdArgs.size() != 2)
	{
		cerr << "smash error: cd: too many agruments" << endl;
		return nullptr;
	}

	return new ChangeDirCommand(cmdStr, cmdArgs[1]);
}

ChangeDirCommand::ChangeDirCommand(const string& cmdStr, const string& path) : InternalCommand(cmdStr)
{
	this->path = Trim(RemoveBackgroundSign(path));
}

void ChangeDirCommand::Execute()
{
	Smash& instance = Smash::Instance();

	char* currPath = get_current_dir_name();

	if (currPath == nullptr)
	{
		SysError("getcwd");
		return;
	}

	int res;

	if (path == "-")
	{
		if (instance.prevPath == "")
		{
			cerr << "smash error: cd: OLDPWD not set" << endl;
			return;
		}

		res = chdir(instance.prevPath.c_str());
	}
	else
	{
		res = chdir(path.c_str());
	}

	if (res < 0)
	{
		SysError("chdir");
		return;
	}

	instance.prevPath = currPath;
}
