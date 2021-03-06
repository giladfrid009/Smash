#include "Special.h"
#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include "Smash.h"
#include <string>
#include <vector>
#include <fcntl.h>
#include <utime.h>

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

static int OpenFile(string path, int flags)
{
	int fd = open(path.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

	if (fd < 0)
	{
		SysError("open");
		return -1;
	}

	return fd;
}

static string ReadStdin()
{
	const size_t ReadSize = 4096 * sizeof(char);

	string output;

	char readBuff[ReadSize];

	ssize_t num;

	while ((num = read(STDIN_FILENO, readBuff, ReadSize)) > 0)
	{
		output.append(readBuff);
	}

	if (num < 0)
	{
		SysError("read");
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
	this->command = Trim(RemoveBackgroundSign(command));
	this->output = Trim(RemoveBackgroundSign(output));
}

void RedirectWriteCommand::Execute()
{
	int fd = OpenFile(output, O_WRONLY | O_CREAT | O_TRUNC);

	if (fd < 0)
	{
		return;
	}

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0)
	{
		SysError("dup");
		close(fd);
		return;
	}

	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		SysError("dup2");
		close(fd);
		close(outCopy);
		return;
	}

	Smash::Instance().Execute(command);

	if (dup2(outCopy, STDOUT_FILENO) < 0)
	{
		SysError("dup2");
	}

	if (close(fd) < 0)
	{
		SysError("close");
	}

	if (close(outCopy) < 0)
	{
		SysError("close");
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
	this->command = Trim(RemoveBackgroundSign(command));
	this->output = Trim(RemoveBackgroundSign(output));
}

void RedirectAppendCommand::Execute()
{
	int fd = OpenFile(output, O_WRONLY | O_CREAT | O_APPEND);

	if (fd < 0)
	{
		return;
	}

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0)
	{
		SysError("dup");
		close(fd);
		return;
	}

	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		SysError("dup2");
		close(fd);
		close(outCopy);
		return;
	}

	Smash::Instance().Execute(command);

	if (dup2(outCopy, STDOUT_FILENO) < 0)
	{
		SysError("dup2");
	}

	if (close(fd) < 0)
	{
		SysError("close");
	}

	if (close(outCopy) < 0)
	{
		SysError("close");
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
	this->left = Trim(RemoveBackgroundSign(left));
	this->right = Trim(RemoveBackgroundSign(right));
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

	pid_t pid = fork();

	if (pid < 0)
	{
		SysError("fork");
		close(readPipe);
		close(writePipe);
		return;
	}

	if (pid == 0)
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

		if (close(readPipe) < 0)
		{
			SysError("close");
			exit(1);
		}

		if (close(writePipe) < 0)
		{
			SysError("close");
			exit(1);
		}

		if (CommandType(right) != Commands::Unknown)
		{
			instance.ExecuteRemote(right + " " + ReadStdin());
		}
		else
		{
			instance.ExecuteRemote(right);
		}

		exit(0);
	}

	int outCopy = dup(STDOUT_FILENO);

	if (outCopy < 0)
	{
		SysError("dup");
		goto ERROR;
	}

	if (close(readPipe) < 0)
	{
		SysError("close");
		goto ERROR;
	}

	if (dup2(writePipe, STDOUT_FILENO) < 0)
	{
		SysError("dup2");
		goto ERROR;
	}

	if (close(writePipe))
	{
		SysError("close");
		goto ERROR;
	}

	instance.Execute(left);

	if (dup2(outCopy, STDOUT_FILENO) < 0)
	{
		SysError("dup2");
	}

	if (close(outCopy) < 0)
	{
		SysError("close");
	}

	if (waitpid(pid, nullptr, 0) < 0)
	{
		SysError("waitpid");
	}

	return;

	ERROR:

	kill(pid, SIGKILL);
	waitpid(pid, nullptr, 0);
	dup2(outCopy, STDOUT_FILENO);
	close(outCopy);
	close(readPipe);
	close(writePipe);
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
	this->left = Trim(RemoveBackgroundSign(left));
	this->right = Trim(RemoveBackgroundSign(right));
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

	pid_t pid = fork();

	if (pid < 0)
	{
		SysError("fork");
		close(readPipe);
		close(writePipe);
		return;
	}

	if (pid == 0)
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

		if (close(readPipe) < 0)
		{
			SysError("close");
			exit(1);
		}

		if (close(writePipe) < 0)
		{
			SysError("close");
			exit(1);
		}

		if (CommandType(right) != Commands::Unknown)
		{
			instance.ExecuteRemote(right + " " + ReadStdin());
		}
		else
		{
			instance.ExecuteRemote(right);
		}

		exit(0);
	}

	int errCopy = dup(STDERR_FILENO);

	if (errCopy < 0)
	{
		SysError("dup");
		goto ERROR;
	}

	if (close(readPipe) < 0)
	{
		SysError("close");
		goto ERROR;
	}

	if (dup2(writePipe, STDERR_FILENO) < 0)
	{
		SysError("dup2");
		goto ERROR;
	}

	if (close(writePipe))
	{
		SysError("close");
		goto ERROR;
	}

	instance.Execute(left);

	if (dup2(errCopy, STDERR_FILENO) < 0)
	{
		SysError("dup2");
	}

	if (close(errCopy) < 0)
	{
		SysError("close");
	}

	if (waitpid(pid, nullptr, 0) < 0)
	{
		SysError("waitpid");
	}

	return;

ERROR:

	kill(pid, SIGKILL);
	waitpid(pid, nullptr, 0);
	dup2(errCopy, STDERR_FILENO);
	close(errCopy);
	close(readPipe);
	close(writePipe);
}

Command* TouchCommand::Create(const string& cmdStr, const vector<string>& cmdArgs)
{
	if (CommandType(cmdArgs) != Commands::Touch)
	{
		return nullptr;
	}

	if (cmdArgs.size() != 3)
	{
		cerr << "smash error: touch: invalid arguments" << endl;
		return nullptr;
	}

	tm timeStruct = {0};

	char* res = strptime(cmdArgs[2].c_str(), "%S:%M:%H:%d:%m:%Y", &timeStruct);

	if (res == nullptr)
	{
		cerr << "smash error: touch: invalid arguments" << endl;
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
	this->path = Trim(path);
	this->time = time;
}

void TouchCommand::Execute()
{
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
			if (cmdArgs[1][0] != '-')
			{
				cerr << "smash error: tail: invalid arguments" << endl;
				return nullptr;
			}

			int count = (-1) * std::stoi(cmdArgs[1]);

			return new TailCommand(cmdStr, cmdArgs[2], count);
		}
	}
	catch (...)
	{
		cerr << "smash error: tail: invalid arguments" << endl;
		return nullptr;
	}

	cerr << "smash error: tail: invalid arguments" << endl;
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
	const char NewLine = '\n';

	int fd = OpenFile(path, O_RDONLY);

	if (fd < 0)
	{
		return;
	}

	char curChar;
	int curLine = 0;

	ssize_t num;

	while ((num = read(fd, &curChar, sizeof(char))) > 0)
	{
		if (curChar == NewLine)
		{
			curLine++;
		}
	}

	if (num < 0)
	{
		SysError("read");
		close(fd);
		return;
	}

	if (curChar != NewLine)
	{
		curLine++;
	}

	if (count > curLine)
	{
		count = curLine;
	}

	int dstLine = curLine - count;

	if (close(fd) < 0)
	{
		SysError("close");
		return;
	}

	fd = OpenFile(path, O_RDONLY);

	if (fd < 0)
	{
		return;
	}

	curLine = 0;

	while ((num = read(fd, &curChar, sizeof(char))) > 0)
	{
		if (curChar == NewLine)
		{
			curLine++;
		}

		if (curLine == dstLine && curChar == NewLine)
		{
			continue;
		}

		if (curLine >= dstLine)
		{
			cout << curChar;
		}
	}

	if (num < 0)
	{
		SysError("read");
	}

	if (close(fd) < 0)
	{
		SysError("close");
	}
}
