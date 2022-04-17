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

	//todo: maybe job can accept ONLY externalCommands. make sure.
	void AddJob(pid_t pid, Command* command, bool isStopped = false);

	void Print();

	void KillAll();

	void RemoveFinished();

	pid_t GetPid(int jobID);

	private:

	int NextID();
};

#endif
