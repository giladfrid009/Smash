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

	JobEntry(int jobId, pid_t pid, Command* command, bool isStopped);

	void UpdateStatus();

	JobStatus Status();

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

	void AddJob(pid_t pid, Command* command, bool isStopped = false);

	void Print();

	void KillAll();

	void UpdateStatus();

	void RemoveFinished();

	private:

	int NextJobId();

	/*JobEntry* GetJobById(int jobId);

	void RemoveJobById(int jobId);

	JobEntry* GetLastJob(int* lastJobId);

	JobEntry* GetLastStoppedJob(int* jobId);*/
};

#endif

