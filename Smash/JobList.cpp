#include "JobList.h"
#include <iostream>
#include <vector>
#include <algorithm>

using std::string;
using std::map;
using std::vector;

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

	int newID = NextID();

	JobEntry job = JobEntry(newID, pid, command, isStopped);

	jobs[newID] = job;
}

void JobsList::PrintJobs() const
{
	vector<int> keys;

	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		keys.push_back(i->first);
	}

	std::sort(keys.begin(), keys.end());

	for (auto i = keys.begin(); i != keys.end(); i++)
	{
		jobs.at(*i).PrintJob();
	}
}

void JobsList::PrintQuit() const
{
	vector<JobEntry> keys;

	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		keys.push_back(i->second);
	}

	std::sort(keys.begin(), keys.end(), [] (const JobEntry& left, const JobEntry& right) {return left.Pid() < right.Pid(); });

	for (auto i = keys.begin(); i != keys.end(); i++)
	{
		i->PrintQuit();
	}
}

void JobsList::KillAll()
{
	//todo: send kill signal to all processes
	// wait for all processes till no-one is left
	// call RemoveFinished()
	// make sure everyone are gone
}

void JobsList::RemoveFinished()
{
	for (auto i = jobs.begin(); i != jobs.end();)
	{
		JobEntry& job = i->second;

		if (job.Status() == JobStatus::Finished)
		{
			job.Destroy();
			i = jobs.erase(i);
		}
		else
		{
			i++;
		}
	}
}

Command* JobsList::Remove(int jobID)
{
	if (jobs.count(jobID) == 0)
	{
		return nullptr;
	}

	Command* cmd = jobs[jobID].CommandPtr();

	jobs.erase(jobID);

	return cmd;
}

int JobsList::NextID() const
{
	int max = MaxID();

	if (max == -1)
	{
		return 1;
	}

	return max + 1;
}

long unsigned int JobsList::Size() const
{
	return jobs.size();
}

int JobsList::MaxID() const
{
	int max = -1;

	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		if (i->first > max)
		{
			max = i->first;
		}
	}

	return max;
}

pid_t JobsList::GetPid(int jobID) const
{
	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		if (i->first == jobID)
		{
			return i->second.Pid();
		}
	}

	return -1;
}

JobStatus JobsList::GetStatus(int jobID)
{
	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		if (i->first == jobID)
		{
			return i->second.Status();
		}
	}

	return JobStatus::Unknown;
}

int JobsList::MaxStopped()
{
	int max = -1;

	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		if (i->second.Status() == JobStatus::Stopped)
		{
			max = i->first;
		}
	}

	return max;
}

void JobsList::SetStatus(int jobID, JobStatus status)
{
	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		if (i->first == jobID)
		{
			i->second.SetStatus(status);
			return;
		}
	}
}
