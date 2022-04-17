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
	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	ExternalCommand(std::string& cmdStr);

	virtual ~ExternalCommand() = default;

	void Execute() override;
};

class InternalCommand : public Command
{
	public:

	InternalCommand(std::string& cmdStr);

	virtual ~InternalCommand() override = default;
};

class SleepPrintCommand : public InternalCommand
{
	protected:

	unsigned int duration;
	std::string messege;

	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	SleepPrintCommand(std::string& cmdStr, int duration, std::string messege);

	virtual ~SleepPrintCommand() override = default;

	void Execute() override;
};

class JobsCommand : public InternalCommand
{
	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	JobsCommand(std::string& cmdStr);

	virtual ~JobsCommand() override = default;

	void Execute() override;
};

class KillCommand : public InternalCommand
{
	protected:

	int signalNum;
	int jobID;

	public:

	static Command* Create(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	KillCommand(std::string& cmdStr, int signalNum, int jobID);

	virtual ~KillCommand() override = default;

	void Execute() override;
};

#endif //SMASH_COMMAND_H
