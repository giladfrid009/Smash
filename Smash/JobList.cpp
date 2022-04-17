#include "JobList.h"
#include <iostream>
#include <vector>
#include <algorithm>

using std::string;
using std::map;
using std::vector;

JobEntry::JobEntry()
{
	jobID = -1;
	pid = -1;
	command = nullptr;
	status = JobStatus::Finished;
	startTime = time(nullptr);
}

JobEntry::JobEntry(int jobID, pid_t pid, Command* command, bool isStopped)
{
	this->jobID = jobID;
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
	else if (result == pid)
	{
		status = JobStatus::Finished;
	}
	else
	{
		perror("smash error: waitpid failed");
	}
}

JobStatus JobEntry::Status()
{
	return status;
}

pid_t JobEntry::Pid()
{
	return pid;
}

void JobEntry::Print()
{
	int diff = (int)difftime(time(nullptr), startTime);

	std::cout << "[" << jobID << "] " << command->ToString() << " : " << diff;

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
	RemoveFinished();

	if (command == nullptr)
	{
		return;
	}

	int newID = NextID();

	JobEntry job = JobEntry(newID, pid, command, isStopped);

	jobs[newID] = job;
}

void JobsList::Print()
{
	RemoveFinished();

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
	UpdateStatus();

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

int JobsList::NextID()
{
	int max = 0;

	for (auto i = jobs.begin(); i != jobs.end(); i++)
	{
		if (i->first > max)
		{
			max = i->first;
		}
	}

	return max + 1;
}

pid_t JobsList::GetPid(int jobID)
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
