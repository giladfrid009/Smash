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

	void AddJob(pid_t pid, Command* command, bool isStopped = false);

	void PrintJobs() const;

	void PrintQuit() const;

	void KillAll(); //todo: is it used anywhere?

	void RemoveFinished();

	Command* Remove(int jobID); //todo: remove later

	pid_t GetPid(int jobID) const;

	JobStatus GetStatus(int jobID);

	void SetStatus(int jobID, JobStatus status);

	void ResetTime(int jobID);

	int MaxStopped();

	int MaxID() const;

	long unsigned int Size() const;

	private:

	int NextID() const;
};

#endif
