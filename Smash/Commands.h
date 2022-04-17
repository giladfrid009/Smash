#ifndef SMASH_COMMAND_H
#define SMASH_COMMAND_H

#include <vector>
#include <string>
#include <sys/wait.h>
#include <cassert>

class Command
{
	protected:

	std::string cmdStr;

	Command(std::string cmdStr);

	public:

	virtual ~Command() = default;

	virtual void Execute() = 0;

	std::string ToString();
};

class ExternalCommand : public Command
{
	ExternalCommand(std::string& cmdStr);

	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	virtual ~ExternalCommand() = default;

	void Execute() override;
};

class InternalCommand : public Command
{
	public:

	InternalCommand(std::string& cmdStr) : Command(cmdStr)
	{
	}

	virtual ~InternalCommand() override = default;
};

class SleepPrintCommand : public InternalCommand
{
	protected:

	unsigned int duration;
	std::string messege;

	SleepPrintCommand(std::string& cmdStr, int duration, std::string messege);

	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	virtual ~SleepPrintCommand() override = default;

	void Execute() override;
};

class JobsCommand : public InternalCommand
{
	protected:

	JobsCommand(std::string& cmdStr);

	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	virtual ~JobsCommand() override = default;

	void Execute() override;
};

class KillCommand : public InternalCommand
{
	protected:

	int signalNum;
	int jobID;

	KillCommand(std::string& cmdStr, int signalNum, int jobID);

	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	virtual ~KillCommand() override = default;

	void Execute() override;
};

#endif //SMASH_COMMAND_H
