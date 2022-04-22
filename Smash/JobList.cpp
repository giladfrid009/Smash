#include "JobList.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using std::string;
using std::map;
using std::vector;
using std::pair;

JobsList::~JobsList()
{
	for (const auto& pair : jobs)
	{
		delete pair.second.CommandPtr();
	}
}

void JobsList::Add(pid_t pid, Command* command, JobStatus status)
{
	if (command == nullptr)
	{
		return;
	}

	int newID = NextID();

	JobEntry job = JobEntry(newID, pid, command, status);

	jobs[newID] = job;
}

void JobsList::ForEach(std::function<void(const JobEntry&)> action) const
{
	vector<JobEntry> sorted;

	for (const auto& pair : jobs)
	{
		sorted.push_back(pair.second);
	}

	std::sort(sorted.begin(), sorted.end(), [] (const JobEntry& left, const JobEntry& right) { return left.ID() < right.ID(); });

	for (const auto& job : sorted)
	{
		action(job);
	}
}

void JobsList::RemoveFinished()
{
	for (auto i = jobs.begin(); i != jobs.end();)
	{
		JobEntry& job = i->second;

		if (job.Status() != JobStatus::Finished)
		{
			i++;
			continue;
		}

		delete job.CommandPtr(); //toto: perhaps re-add JobEntry.Destroy

		i = jobs.erase(i);
	}
}

void JobsList::ResetTime(int jobID)
{
	if (jobs.count(jobID) == 0)
	{
		return;
	}

	jobs.at(jobID).ResetTime();
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

	for (const auto& pair : jobs)
	{
		if (pair.first > max)
		{
			max = pair.first;
		}
	}

	return max;
}

pid_t JobsList::GetPID(int jobID) const
{
	if (jobs.count(jobID) == 0)
	{
		return -1;
	}

	return jobs.at(jobID).PID();
}

JobStatus JobsList::GetStatus(int jobID)
{
	if (jobs.count(jobID) == 0)
	{
		return JobStatus::Unknown;
	}

	return jobs.at(jobID).Status();
}

int JobsList::MaxStopped()
{
	int max = -1;

	for (auto& pair : jobs)
	{
		if (pair.second.Status() == JobStatus::Stopped)
		{
			max = pair.first;
		}
	}

	return max;
}

void JobsList::SetStatus(int jobID, JobStatus status)
{
	if (jobs.count(jobID) == 0)
	{
		return;
	}

	return jobs.at(jobID).SetStatus(status);
}
