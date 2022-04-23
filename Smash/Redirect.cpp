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
	Smash& instance = Smash::Instance();

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0)
	{
		SysError("dup");
		return;
	}

	int res = close(STDOUT_FILENO);

	if (res < 0)
	{
		SysError("close");
		close(outCopy);
		return;
	}

	int fd = open(output.c_str(), O_WRONLY | O_CREAT | O_TRUNC);

	if (fd < 0 || fd != STDOUT_FILENO)
	{
		SysError("open");
		close(fd);
		dup2(outCopy, STDOUT_FILENO);
		close(outCopy);
		return;
	}

	instance.Execute(command);

	res = dup2(outCopy, STDOUT_FILENO);

	if (res < 0) SysError("dup2");

	res = close(outCopy);

	if (res < 0) SysError("close");
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

	if (outCopy < 0)
	{
		SysError("dup");
		return;
	}

	int res = close(STDOUT_FILENO);

	if (res < 0)
	{
		SysError("close");
		close(outCopy);
		return;
	}

	int fd = open(output.c_str(), O_WRONLY | O_APPEND | O_CREAT);

	if (fd < 0 || fd != STDOUT_FILENO)
	{
		SysError("open");
		close(fd);
		dup2(outCopy, STDOUT_FILENO);
		close(outCopy);
		return;
	}

	instance.Execute(command);

	res = dup2(outCopy, STDOUT_FILENO);

	if (res < 0) SysError("dup2");

	res = close(outCopy);

	if (res < 0) SysError("close");
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

	int inCopy = dup(STDIN_FILENO);

	if (inCopy < 0)
	{
		SysError("dup");
		return;
	}

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0)
	{
		SysError("dup");
		close(inCopy);
		return;
	}

	int pipeFds[2];

	int res = pipe(pipeFds);

	if (res < 0)
	{
		SysError("pipe");
		close(inCopy);
		close(outCopy);
		return;
	}

	int readPipe = pipeFds[0];
	int writePipe = pipeFds[1];

	res = dup2(writePipe, STDOUT_FILENO);

	if (res < 0)
	{
		SysError("dup2");
		close(writePipe);
		goto CLEANUP;
	}

	instance.Execute(left);

	res = dup2(outCopy, STDOUT_FILENO);

	if (res < 0)
	{
		SysError("dup2");
		close(STDOUT_FILENO);
		close(writePipe);
		goto CLEANUP;
	}

	res = close(writePipe);

	if (res < 0)
	{
		SysError("close");
		goto CLEANUP;
		return;
	}

	res = dup2(readPipe, STDIN_FILENO);

	if (res < 0)
	{
		SysError("dup2");
		goto CLEANUP;
	}

	if (CommandType(right) != Commands::Unknown)
	{
		instance.Execute(right + " " + ReadStdin());
	}
	else
	{
		instance.Execute(right);
	}

	res = dup2(inCopy, STDIN_FILENO);

	if (res < 0)
	{
		close(STDIN_FILENO);
		SysError("dup2");
	}

CLEANUP:

	res = close(readPipe);

	if (res < 0) SysError("close");

	res = close(inCopy);

	if (res < 0) SysError("close");

	res = close(outCopy);

	if (res < 0) SysError("close");
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

	if (inCopy < 0)
	{
		SysError("dup");
		return;
	}

	int errCopy = dup(STDERR_FILENO);

	if (errCopy < 0)
	{
		SysError("dup");
		close(inCopy);
		return;
	}

	int pipeFds[2];

	int res = pipe(pipeFds);

	if (res < 0)
	{
		SysError("pipe");
		close(inCopy);
		close(errCopy);
		return;
	}

	int readPipe = pipeFds[0];
	int writePipe = pipeFds[1];

	res = dup2(writePipe, STDERR_FILENO);

	if (res < 0)
	{
		SysError("dup2");
		close(writePipe);
		goto CLEANUP;
	}

	instance.Execute(left);

	res = dup2(errCopy, STDERR_FILENO);

	if (res < 0)
	{
		SysError("dup2");
		close(STDERR_FILENO);
		close(writePipe);
		goto CLEANUP;
	}

	res = close(writePipe);

	if (res < 0)
	{
		SysError("close");
		goto CLEANUP;
		return;
	}

	res = dup2(readPipe, STDIN_FILENO);

	if (res < 0)
	{
		SysError("dup2");
		goto CLEANUP;
	}

	if (CommandType(right) != Commands::Unknown)
	{
		instance.Execute(right + " " + ReadStdin());
	}
	else
	{
		instance.Execute(right);
	}

	res = dup2(inCopy, STDIN_FILENO);

	if (res < 0)
	{
		close(STDIN_FILENO);
		SysError("dup2");
	}

CLEANUP:

	res = close(readPipe);

	if (res < 0) SysError("close");

	res = close(inCopy);

	if (res < 0) SysError("close");

	res = close(errCopy);

	if (res < 0) SysError("close");
}
