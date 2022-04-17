#ifndef JOBENTRY_H
#define JOBENTRY_H

#include "Commands.h"

enum class JobStatus
{
	Running,
	Stopped,
	Finished
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

	JobStatus Status();

	pid_t Pid();

	void Print();

	void Destroy();
};

#endif // !JOBENTRY_H
