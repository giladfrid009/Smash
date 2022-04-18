#include "JobEntry.h"
#include <iostream>

JobEntry::JobEntry()
{
	jobID = -1;
	pid = -1;
	command = nullptr;
	status = JobStatus::Unknown;
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

void JobEntry::SetStatus(JobStatus status)
{
	this->status = status;
}

JobStatus JobEntry::Status()
{
	//todo: add Stop() and Continue() for JobList
	//todo: how SIGCONT and SIGTSTP are sent?

	// jobs stopped = SIGTSTP
	// jobs continued = SIGOCONT

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