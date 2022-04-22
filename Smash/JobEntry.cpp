#include "JobEntry.h"
#include <iostream>
#include <string>

JobEntry::JobEntry()
{
	jobID = -1;
	pid = -1;
	command = nullptr;
	status = JobStatus::Unknown;
	startTime = time(nullptr);
}

JobEntry::JobEntry(int jobID, pid_t pid, Command* command, JobStatus status)
{
	this->jobID = jobID;
	this->pid = pid;
	this->command = command;
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

Command* JobEntry::CommandPtr() const
{
	return command;
}

void JobEntry::ResetTime()
{
	startTime = time(nullptr);
}

void JobEntry::PrintJob() const
{
	if (command == nullptr)
	{
		return;
	}

	int diff = (int)difftime(time(nullptr), startTime);

	std::cout << "[" << jobID << "] " << command->ToString() << " : " << pid << " " << diff << " secs";

	if (status == JobStatus::Stopped)
	{
		std::cout << " (stopped)";
	}

	std::cout << std::endl;
}

void JobEntry::PrintQuit() const
{
	if (command == nullptr)
	{
		return;
	}

	std::cout << pid << ": " << command->ToString() << std::endl;
}

void JobEntry::PrintCommand() const
{
	if (command != nullptr)
	{
		std::cout << command->ToString();
	}
}
