#ifndef JOBLIST_H
#define JOBLIST_H

#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include <time.h>
#include <map>

enum class JobStatus
{
	Running,
	Stopped,
	Finished
};

class JobEntry
{
	private:

	int jobId;
	pid_t pid;
	Command* command;
	JobStatus status;
	time_t startTime;

	public:

	JobEntry();

	//todo: maybe jobEntry can accept ONLY externalCommands. make sure.
	JobEntry(int jobId, pid_t pid, Command* command, bool isStopped);

	void UpdateStatus();

	JobStatus Status();

	pid_t Pid();

	void Print();

	void Destroy();
};

class JobsList
{
	private:

	std::map<int, JobEntry> jobs;

	public:

	JobsList() = default;

	~JobsList();

	//todo: maybe job can accept ONLY externalCommands. make sure.
	void AddJob(pid_t pid, Command* command, bool isStopped = false);

	void Print();

	void KillAll();

	void UpdateStatus();

	void RemoveFinished();

	pid_t GetPid(int jobId);

	private:

	int NextJobId();
};

#endif

