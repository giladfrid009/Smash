#ifndef SMASH_COMMAND_H
#define SMASH_COMMAND_H

#include <vector>
#include <string>

class Command
{
	public:

	Command()
	{
	}

	virtual ~Command() = default;

	virtual void Execute() = 0;
};

class ExternalCommand : public Command
{
	private:

	std::string cmdStr;

	ExternalCommand(std::string& cmdStr);

	public:

	static Command* Create(std::string& cmdArgs);

	virtual ~ExternalCommand() = default;

	void Execute() override;
};

class InternalCommand : public Command
{
	public:

	InternalCommand() : Command()
	{
	}

	virtual ~InternalCommand() override = default;
};

class SleepPrintCommand : public InternalCommand
{
	private:

	unsigned int duration;
	std::string messege;

	SleepPrintCommand(int duration, std::string messege);

	public:

	static Command* Create(std::vector<std::string>& cmdArgs);

	virtual ~SleepPrintCommand() override = default;

	void Execute() override;
};

class JobsList
{
	public:

	class JobEntry
	{
	};

	public:

	JobsList();

	~JobsList();

	void AddJob(Command* cmd, bool isStopped = false);

	void PrintJobsList();

	void KillAllJobs();

	void RemoveFinishedJobs();

	JobEntry* GetJobById(int jobId);

	void RemoveJobById(int jobId);

	JobEntry* GetLastJob(int* lastJobId);

	JobEntry* GetLastStoppedJob(int* jobId);
};

#endif //SMASH_COMMAND_H_
