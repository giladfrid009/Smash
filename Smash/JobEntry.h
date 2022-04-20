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

	JobEntry(int jobID, pid_t pid, Command* command, JobStatus status);

	void SetStatus(JobStatus status);

	JobStatus Status();

	pid_t PID() const;

	int ID() const;

	Command* CommandPtr() const;

	void ResetTime();

	void PrintJob() const;

	void PrintQuit() const;
};

#endif // !JOBENTRY_H
