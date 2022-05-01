#ifndef JOBENTRY_H
#define JOBENTRY_H

#include "Commands.h"
#include <string>

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
	std::string cmdStr;
	JobStatus status;
	time_t startTime;

	public:

	JobEntry();

	JobEntry(int jobID, pid_t pid, const std::string& cmdStr, JobStatus status);

	void SetStatus(JobStatus status);

	JobStatus Status();

	pid_t PID() const;

	int ID() const;

	void ResetTime();

	void PrintJob() const;

	void PrintQuit() const;

	std::string CommandStr() const;
};

#endif // !JOBENTRY_H
