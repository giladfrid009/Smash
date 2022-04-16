#ifndef SMASH_COMMAND_H
#define SMASH_COMMAND_H

#include <vector>
#include <string>

class Command
{
	public:

	Command(const std::string& cmdStr);

	virtual ~Command();

	virtual void Execute() = 0;
};

class ExternalCommand : public Command
{
	public:

	ExternalCommand(const std::string& cmdStr);

	virtual ~ExternalCommand() {}

	void Execute() override;
};

class InternalCommand : public Command
{
	public:

	InternalCommand(const std::string& cmdStr);

	virtual ~InternalCommand() {}
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
