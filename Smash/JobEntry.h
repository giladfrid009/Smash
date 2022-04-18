#ifndef JOBENTRY_H
#define JOBENTRY_H

#include "Commands.h"

enum class JobStatus
{
	Running,
	Stopped,
	Finished,
	Unknown
};

class JobEntry
{
	private:

	int jobID;
	pid_t pid;
	Command* command;
	JobStatus status;
	time_t startTime;

	public:

	JobEntry();

	//todo: can accept ONLY externalCommands
	JobEntry(int jobID, pid_t pid, Command* command, bool isStopped);

	void SetStatus(JobStatus status);

	JobStatus Status();

	pid_t Pid() const;

	Command* CommandPtr();

	void PrintJob();

	void PrintQuit();

	void Destroy();
};

#endif // !JOBENTRY_H
