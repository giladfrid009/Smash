#include "JobList.h"
#include <iostream>
#include <vector>
#include <algorithm>

using std::string;
using std::map;
using std::vector;

JobEntry::JobEntry()
{
	jobId = -1;
	pid = -1;
	command = nullptr;
	status = JobStatus::Finished;
	this->startTime = time(nullptr);
}

JobEntry::JobEntry(int jobId, pid_t pid, Command* command, bool isStopped)
{
	this->jobId = jobId;
	this->pid = pid;
	this->command = command;
	this->status = isStopped ? JobStatus::Stopped : JobStatus::Running;
	this->startTime = time(nullptr);
}

void JobEntry::UpdateStatus()
{
	pid_t result = waitpid(pid, nullptr, WNOHANG);

	if (result == 0)
	{
		return;
	}
	else if (result > 0)
	{
		assert(result == pid);
		status = JobStatus::Finished;
	}
	else
	{
		//todo: handle error correctly
		perror("smash error: bad wait pid check");
	}
}

JobStatus JobEntry::Status()
{
	return status;
}

void JobEntry::Print()
{
	int diffTime = (int)difftime(time(nullptr), startTime);

	std::cout << "[" << jobId << "] " << command->CommandString() << " : " << pid << " " << diffTime;

	if (status == JobStatus::Stopped)
	{
		std::cout << " (stopped)";
	}

	std::cout << std::endl;
}

void JobEntry::Destroy()
{
	//todo: may cause problems later in bg or fg
	delete command;
}

JobsList::~JobsList()
{
	KillAll();
}

void JobsList::AddJob(pid_t pid, Command* command, bool isStopped)
{
	if (command == nullptr)
	{
		return;
	}

	int jobId = NextJobId();

	JobEntry job(jobId, pid, command, isStopped);

	jobs[jobId] = job;
}

void JobsList::Print()
{
	vector <int> keys;

	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		keys.push_back(i->first);
	}

	std::sort(keys.begin(), keys.end());

	for (auto i = keys.begin(); i != keys.end(); i++)
	{
		jobs[*i].Print();
	}
}

void JobsList::KillAll()
{
	//todo: send kill signal to all processes
	// wait for all processes till no-one is left
	// call UpdateStatus
	// make sure everyone are stopped
	// remove all entries from jobs
}

void JobsList::UpdateStatus()
{
	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		i->second.UpdateStatus();
	}
}

void JobsList::RemoveFinished()
{
	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		JobEntry job = i->second;

		if (job.Status() == JobStatus::Finished)
		{
			job.Destroy();
			jobs.erase(i->first);
		}
	}
}

int JobsList::NextJobId()
{
	int max = 1;

	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		if (i->first > max)
		{
			max = i->first;
		}
	}

	return max;
}
