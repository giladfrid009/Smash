#include "Redirect.h"
#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include "Smash.h"
#include <string>
#include <vector>
#include <fcntl.h>

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

static string ReadStdin()
{
	const size_t ReadSize = 4096 * sizeof(char);

	string output;

	char readBuff[ReadSize];

	while (read(STDIN_FILENO, readBuff, ReadSize) > 0)
	{
		output.append(readBuff);
	}

	return output;
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
	pid_t pid = fork();

	if (pid < 0)
	{
		SysError("fork");
		return;
	}

	if (pid == 0)
	{
		if (setpgrp() < 0)
		{
			SysError("setpgrp");
			exit(1);
		}

		int fd = open(output.c_str(), O_WRONLY | O_CREAT | O_TRUNC);

		if (fd < 0)
		{
			SysError("open");
			exit(1);
		}

		if (dup2(fd, STDOUT_FILENO) < 0)
		{
			SysError("dup2");
			exit(1);
		}

		if (close(fd) < 0)
		{
			SysError("close");
		}

		Smash::Instance().Execute(command);

		exit(0);
	}

	if (waitpid(pid, nullptr, 0) < 0)
	{
		SysError("waitpid");
	}
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
	pid_t pid = fork();

	if (pid < 0)
	{
		SysError("fork");
		return;
	}

	if (pid == 0)
	{
		if (setpgrp() < 0)
		{
			SysError("setpgrp");
			exit(1);
		}

		int fd = open(output.c_str(), O_WRONLY | O_CREAT | O_APPEND);

		if (fd < 0)
		{
			SysError("open");
			exit(1);
		}

		if (dup2(fd, STDOUT_FILENO) < 0)
		{
			SysError("dup2");
			exit(1);
		}

		if (close(fd) < 0)
		{
			SysError("dup2");
		}

		Smash::Instance().Execute(command);

		exit(0);
	}

	if (waitpid(pid, nullptr, 0) < 0)
	{
		SysError("waitpid");
	}
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

void PipeOutCommand::Execute()
{
	Smash& instance = Smash::Instance();

	int fds[2];

	if (pipe(fds) < 0)
	{
		SysError("pipe");
		return;
	}

	int readPipe = fds[0];
	int writePipe = fds[1];

	pid_t leftPID = fork();

	if (leftPID < 0)
	{
		SysError("fork");
		close(readPipe);
		close(writePipe);
		return;
	}

	if (leftPID == 0)
	{
		if (setpgrp() < 0)
		{
			SysError("setpgrp");
			exit(1);
		}

		if (dup2(writePipe, STDOUT_FILENO) < 0)
		{
			SysError("dup2");
			exit(1);
		}

		close(readPipe);
		close(writePipe);

		instance.Execute(left);

		exit(0);
	}

	pid_t rightPID = fork();

	if (rightPID < 0)
	{
		SysError("fork");
		close(readPipe);
		close(writePipe);
		return;
	}

	if (rightPID == 0)
	{
		if (setpgrp() < 0)
		{
			SysError("setpgrp");
			exit(1);
		}

		if (dup2(readPipe, STDIN_FILENO) < 0)
		{
			SysError("dup2");
			exit(1);
		}

		close(readPipe);
		close(writePipe);

		if (CommandType(right) != Commands::Unknown)
		{
			instance.Execute(right + " " + ReadStdin());
		}
		else
		{
			instance.Execute(right);
		}

		exit(0);
	}

	if (close(readPipe) < 0) SysError("close");

	if (close(writePipe) < 0) SysError("close");

	if (waitpid(leftPID, nullptr, 0) < 0) SysError("waitpid");

	if (waitpid(rightPID, nullptr, 0) < 0) SysError("waitpid");
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

	int fds[2];

	if (pipe(fds) < 0)
	{
		SysError("pipe");
		return;
	}

	int readPipe = fds[0];
	int writePipe = fds[1];

	pid_t leftPID = fork();

	if (leftPID < 0)
	{
		SysError("fork");
		close(readPipe);
		close(writePipe);
		return;
	}

	if (leftPID == 0)
	{
		if (setpgrp() < 0)
		{
			SysError("setpgrp");
			exit(1);
		}

		if (dup2(writePipe, STDERR_FILENO) < 0)
		{
			SysError("dup2");
			exit(1);
		}

		close(readPipe);
		close(writePipe);

		instance.Execute(left);

		exit(0);
	}

	pid_t rightPID = fork();

	if (rightPID < 0)
	{
		SysError("fork");
		close(readPipe);
		close(writePipe);
		return;
	}

	if (rightPID == 0)
	{
		if (setpgrp() < 0)
		{
			SysError("setpgrp");
			exit(1);
		}

		if (dup2(readPipe, STDIN_FILENO) < 0)
		{
			SysError("dup2");
			exit(1);
		}

		close(readPipe);
		close(writePipe);

		if (CommandType(right) != Commands::Unknown)
		{
			instance.Execute(right + " " + ReadStdin());
		}
		else
		{
			instance.Execute(right);
		}

		exit(0);
	}

	if (close(readPipe) < 0) SysError("close");

	if (close(writePipe) < 0) SysError("close");

	if (waitpid(leftPID, nullptr, 0) < 0) SysError("waitpid");

	if (waitpid(rightPID, nullptr, 0) < 0) SysError("waitpid");
}
