#ifndef JOBLIST_H
#define JOBLIST_H

#include "JobEntry.h"
#include "Commands.h"
#include "Identifiers.h"
#include "Parser.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include <time.h>
#include <map>

class JobsList
{
	private:

	std::map<int, JobEntry> jobs;

	public:

	JobsList() = default;

	~JobsList();

	//todo: can accept ONLY ExternalCommands
	void AddJob(pid_t pid, Command* command, bool isStopped = false);

	void Print();

	void KillAll();

	void RemoveFinished();

	pid_t GetPid(int jobID);

	JobStatus GetStatus(int jobID);

	int MaxStopped();

	void SetStatus(int jobID, JobStatus status);

	private:

	int NextID();
};

#endif
