#include "JobEntry.h"
#include <iostream>
#include <string>

using std::string;

JobEntry::JobEntry()
{
	jobID = -1;
	pid = -1;
	cmdStr = "";
	status = JobStatus::Unknown;
	startTime = time(nullptr);
}

JobEntry::JobEntry(int jobID, pid_t pid, const string& cmdStr, JobStatus status)
{
	this->jobID = jobID;
	this->pid = pid;
	this->cmdStr = cmdStr;
	this->status = status;
	this->startTime = time(nullptr);
}

void JobEntry::SetStatus(JobStatus status)
{
	this->status = status;
}

JobStatus JobEntry::Status()
{
	if (status == JobStatus::Finished || status == JobStatus::Unknown)
	{
		return status;
	}

	pid_t result = waitpid(pid, nullptr, WNOHANG);

	if (result < 0)
	{
		perror("smash error: waitpid failed");
	}

	if (result == pid)
	{
		status = JobStatus::Finished;
	}

	return status;
}

pid_t JobEntry::PID() const
{
	return pid;
}

int JobEntry::ID() const
{
	return jobID;
}

void JobEntry::ResetTime()
{
	startTime = time(nullptr);
}

void JobEntry::PrintJob() const
{
	int diff = (int)difftime(time(nullptr), startTime);

	std::cout << "[" << jobID << "] " << cmdStr << " : " << pid << " " << diff << " secs";

	if (status == JobStatus::Stopped)
	{
		std::cout << " (stopped)";
	}

	std::cout << std::endl;
}

void JobEntry::PrintQuit() const
{
	std::cout << pid << ": " << cmdStr << std::endl;
}

string JobEntry::CommandStr() const
{
	return cmdStr;	
}
