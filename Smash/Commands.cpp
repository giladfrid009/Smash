#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include "Smash.h"
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <signal.h>
#include <exception>
#include <fcntl.h>
#include <utime.h>
#include <fstream>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

const size_t ReadSize = 4096 * sizeof(char);

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

	char* const args[] = {"bash", "-c", &formatted[0], NULL};

	execv("/bin/bash", args);

	SysError("execv");

	exit(0);
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
		cout << "smash error: kill: invalid arguments" << endl;
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
		cout << "smash error: kill: invalid arguments" << endl;
		return nullptr;
	}
}

void KillCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int pid = instance.jobs.GetPID(jobID);

	if (pid < 0)
	{
		cout << "smash error: kill: job-id " << jobID << " does not exist" << endl;
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
			cout << "smash error: bg: invalid arguments" << endl;
			return nullptr;
		}
	}

	cout << "smash error: bg: invalid arguments" << endl;

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
			cout << "smash error: bg: job-id " << jobID << " does not exist" << endl;
			return;
		}

		if (status == JobStatus::Running)
		{
			cout << "smash error: bg: job-id " << jobID << " is already running in the background" << endl;
			return;
		}

		dstID = jobID;
	}
	else
	{
		dstID = instance.jobs.MaxStopped();

		if (dstID == -1)
		{
			cout << "smash error: bg: there is no stopped jobs to resume" << endl;
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
	pid_t pid = getpid();

	if (pid < 0)
	{
		SysError("getpid");
		return;
	}

	cout << "smash pid is " << pid << endl;
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
			cout << "smash error: fg: invalid arguments" << endl;
			return nullptr;
		}
	}

	cout << "smash error: fg: invalid arguments" << endl;

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
			cout << "smash error: fg: job-id " << jobID << " does not exist" << endl;
			return;
		}

		dstID = jobID;
	}
	else
	{
		dstID = instance.jobs.MaxID();

		if (dstID == -1)
		{
			cout << "smash error: fg: jobs list is empty" << endl;
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

		instance.jobs.ForEach([] (const JobEntry& job) { int res = kill(job.PID(), SIGKILL); if (res < 0) { SysError("kill"); } });
	}

	exit(0);
}

Command* RedirectWriteCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::RedirectWrite)
	{
		return nullptr;
	}

	Identifiers I;

	vector<string> args = Split(cmdStr, I.RedirectWrite);

	if (args.size() != 2)
	{
		return nullptr;
	}

	return new RedirectWriteCommand(cmdStr, args[0], args[1]);
}

RedirectWriteCommand::RedirectWriteCommand(const string& cmdStr, const string& command, const string& output) : InternalCommand(cmdStr)
{
	this->command = RemoveBackgroundSign(command);
	this->output = Trim(output);
}

void RedirectWriteCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0) { SysError("dup"); return; }

	int res = close(STDOUT_FILENO);

	if (res < 0) { SysError("close"); return; }

	res = open(output.c_str(), O_WRONLY | O_CREAT | O_TRUNC);

	if (res < 0 || res != STDOUT_FILENO) { SysError("open"); return; }

	instance.Execute(command);

	res = dup2(outCopy, STDOUT_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(outCopy);
}

Command* RedirectAppendCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::RedirectAppend)
	{
		return nullptr;
	}

	Identifiers I;

	vector<string> args = Split(cmdStr, I.RedirectAppend);

	if (args.size() != 2)
	{
		return nullptr;
	}

	return new RedirectAppendCommand(cmdStr, args[0], args[1]);
}

RedirectAppendCommand::RedirectAppendCommand(const string& cmdStr, const string& command, const string& output) : InternalCommand(cmdStr)
{
	this->command = RemoveBackgroundSign(command);
	this->output = Trim(output);
}

void RedirectAppendCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0) { SysError("dup"); return; }

	int res = close(STDOUT_FILENO);

	if (res < 0) { SysError("close"); return; }

	res = open(output.c_str(), O_WRONLY | O_APPEND | O_CREAT);

	if (res < 0 || res != STDOUT_FILENO) { SysError("open"); return; }

	instance.Execute(command);

	res = dup2(outCopy, STDOUT_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(outCopy);
}

Command* PipeOutCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::PipeOut)
	{
		return nullptr;
	}

	Identifiers I;

	vector<string> args = Split(cmdStr, I.PipeOut);

	if (args.size() != 2)
	{
		return nullptr;
	}

	return new PipeOutCommand(cmdStr, args[0], args[1]);
}

PipeOutCommand::PipeOutCommand(const string& cmdStr, const string& left, const string& right) : InternalCommand(cmdStr)
{
	this->left = RemoveBackgroundSign(left);
	this->right = RemoveBackgroundSign(right);
}

static string ReadStdin()
{
	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

	string output;

	char readBuff[ReadSize];

	while (read(STDIN_FILENO, readBuff, ReadSize) > 0)
	{
		output.append(readBuff);
	}

	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & (~O_NONBLOCK));

	return output;
}

void PipeOutCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int inCopy = dup(STDIN_FILENO);

	if (inCopy < 0) { SysError("dup"); return; }

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0) { SysError("dup"); return; }

	int pipeFds[2];

	int res = pipe(pipeFds);

	if (res < 0) { SysError("pipe"); return; }

	int readPipe = pipeFds[0];
	int writePipe = pipeFds[1];

	res = dup2(writePipe, STDOUT_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(writePipe);

	if (res < 0) { SysError("close"); return; }

	instance.Execute(left);

	res = dup2(outCopy, STDOUT_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(outCopy);

	if (res < 0) { SysError("close"); return; }

	res = dup2(readPipe, STDIN_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(readPipe);

	if (res < 0) { SysError("close"); return; }

	if (CommandType(right) != Commands::Unknown)
	{
		instance.Execute(right + " " + ReadStdin());
	}
	else
	{
		instance.Execute(right);
	}

	res = dup2(inCopy, STDIN_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(inCopy);

	if (res < 0) { SysError("close"); return; }
}

Command* PipeErrCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::PipeErr)
	{
		return nullptr;
	}

	Identifiers I;

	vector<string> args = Split(cmdStr, I.PipeErr);

	if (args.size() != 2)
	{
		return nullptr;
	}

	return new PipeErrCommand(cmdStr, args[0], args[1]);
}

PipeErrCommand::PipeErrCommand(const string& cmdStr, const string& left, const string& right) : InternalCommand(cmdStr)
{
	this->left = RemoveBackgroundSign(left);
	this->right = RemoveBackgroundSign(right);
}

void PipeErrCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int inCopy = dup(STDIN_FILENO);

	if (inCopy < 0) { SysError("dup"); return; }

	int errCopy = dup(STDERR_FILENO);

	if (errCopy < 0) { SysError("dup"); return; }

	int pipeFds[2];

	int res = pipe(pipeFds);

	if (res < 0) { SysError("pipe"); return; }

	int readPipe = pipeFds[0];
	int writePipe = pipeFds[1];

	res = dup2(writePipe, STDERR_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(writePipe);

	if (res < 0) { SysError("close"); return; }

	instance.Execute(left);

	res = dup2(errCopy, STDERR_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(errCopy);

	if (res < 0) { SysError("close"); return; }

	res = dup2(readPipe, STDIN_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(readPipe);

	if (res < 0) { SysError("close"); return; }

	if (CommandType(right) != Commands::Unknown)
	{
		instance.Execute(right + " " + ReadStdin());
	}
	else
	{
		instance.Execute(right);
	}

	res = dup2(inCopy, STDIN_FILENO);

	if (res < 0) { SysError("dup2"); return; }

	res = close(inCopy);

	if (res < 0) { SysError("close"); return; }
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
		cout << "smash error: cd: too many agruments" << endl;
		return nullptr;
	}

	return new ChangeDirCommand(cmdStr, cmdArgs[1]);
}

ChangeDirCommand::ChangeDirCommand(const string& cmdStr, const string& path) : InternalCommand(cmdStr)
{
	this->path = Trim(path);
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
			cout << "smash error: cd: OLDPWD not set" << endl;
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

Command* TouchCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Touch)
	{
		return nullptr;
	}

	if (cmdArgs.size() != 3)
	{
		cout << "smash error: touch: invalid arguments" << endl;
		return nullptr;
	}

	tm timeStruct = {0};

	char* res = strptime(cmdArgs[2].c_str(), "%S:%M:%H:%d:%m:%Y", &timeStruct);

	if (res == nullptr)
	{
		cout << "smash error: touch: invalid arguments" << endl;
		return nullptr;
	}

	time_t time = mktime(&timeStruct);

	if (time == -1)
	{
		return nullptr;
	}

	return new TouchCommand(cmdStr, cmdArgs[1], time);
}

TouchCommand::TouchCommand(const string& cmdStr, const string& path, time_t time) : InternalCommand(cmdStr)
{
	this->path = path;
	this->time = time;
}

void TouchCommand::Execute()
{
	//todo: broken before 1970 in linux

	utimbuf fileTimes{.actime = time, .modtime = time};

	int res = utime(path.c_str(), &fileTimes);

	if (res < 0)
	{
		SysError("utime");
	}
}

Command* TailCommand::Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Tail)
	{
		return nullptr;
	}

	try
	{
		if (cmdArgs.size() == 2)
		{
			return new TailCommand(cmdStr, cmdArgs[1]);
		}

		if (cmdArgs.size() == 3)
		{
			int count = (-1) * std::stoi(cmdArgs[1]);

			return new TailCommand(cmdStr, cmdArgs[2], count);
		}
	}
	catch (...)
	{
		cout << "smash error: tail: invalid arguments" << endl;
		return nullptr;
	}

	cout << "smash error: tail: invalid arguments" << endl;
	return nullptr;
}

TailCommand::TailCommand(const std::string& cmdStr, const std::string& path, int count) : InternalCommand(cmdStr)
{
	if (count < 0)
	{
		throw std::invalid_argument("count");
	}

	this->path = path;
	this->count = count;
}

void TailCommand::Execute()
{
	int fd = open(path.c_str(), O_RDONLY);

	if (fd < 0) { SysError("open"); return; }

	char curChar;
	int curLine = 0;

	while (read(fd, &curChar, 1 * sizeof(char)))
	{
		if (curChar == '\n')
		{
			curLine++;
		}
	}

	if (count > curLine)
	{
		count = curLine;
	}

	int dstLine = curLine - count;

	int res = close(fd);

	if (res < 0) { SysError("close"); return; }

	fd = open(path.c_str(), O_RDONLY);

	if (fd < 0) { SysError("open"); return; }

	curLine = 0;

	while (read(fd, &curChar, 1 * sizeof(char)))
	{
		if (curChar == '\n')
		{
			curLine++;
		}

		if (curLine == dstLine && curChar == '\n')
		{
			continue;
		}

		if (curLine >= dstLine)
		{
			cout << curChar;
		}
	}

	res = close(fd);

	if (res < 0) { SysError("close"); }
}
