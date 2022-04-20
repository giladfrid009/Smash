#include "SigHandlers.h"
#include "Smash.h"
#include <signal.h>
#include <iostream>

void Handler_CtrlZ(int signalNum)
{
	if (signalNum != SIGTSTP)
	{
		return;
	}

	std::cout << "smash: got ctrl-Z" << std::endl;

	Smash& instance = Smash::Instance();
	pid_t pid = instance.CurrentPid();

	if (pid < 0)
	{
		return;
	}

	int res = kill(pid, SIGSTOP);

	if (res < 0)
	{
		perror("smash error: kill failed");
		return;
	}

	std::cout << "smash: process " << pid << " was stopped" << std::endl;
}

void Handler_CtrlC(int signalNum)
{
	if (signalNum != SIGINT)
	{
		return;
	}

	std::cout << "smash: got ctrl-C" << std::endl;

	Smash& instance = Smash::Instance();
	pid_t pid = instance.CurrentPid();

	if (pid < 0)
	{
		return;
	}

	int res = kill(pid, SIGKILL);

	if (res < 0)
	{
		perror("smash error: kill failed");
		return;
	}

	std::cout << "smash: process " << pid << " was killed" << std::endl;
}

void Handler_Alarm(int signalNum)
{
	if (signalNum != SIGALRM)
	{
		return;
	}

	std::cout << "smash: got an alarm" << std::endl;
}