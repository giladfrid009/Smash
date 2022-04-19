
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
#include <functional>

class JobsList
{
	private:

	std::map<int, JobEntry> jobs;

	public:

	JobsList() = default;

	~JobsList();

	void AddJob(pid_t pid, Command* command, bool isStopped = false); //todo: change isStopped to enum JobStatus with default value? 

	long unsigned int Size() const;

	void ForEach(std::function<void(const JobEntry&)> action) const;

	void RemoveFinished();

	pid_t GetPid(int jobID) const;

	JobStatus GetStatus(int jobID);

	void SetStatus(int jobID, JobStatus status);

	void ResetTime(int jobID);

	int MaxStopped();

	int MaxID() const;

	private:

	int NextID() const;
};

#endif
