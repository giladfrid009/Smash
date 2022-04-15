#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Smash.h"
#include "SigHandlers.h"

int main(int argc, char* argv[])
{
	if (signal(SIGTSTP, Handler_CtrlZ) == SIG_ERR)
	{
		perror("smash error: failed to set ctrl-Z handler");
	}

	if (signal(SIGINT, Handler_CtrlC) == SIG_ERR)
	{
		perror("smash error: failed to set ctrl-C handler");
	}

	if (signal(SIGALRM, Handler_Alarm) == SIG_ERR)
	{
		perror("smash error: failed to set alarm handler");
	}

	Smash& smash = Smash::GetInstance();

	while (true)
	{
		std::cout << smash.GetPrompt();
		std::string cmdStr;
		std::getline(std::cin, cmdStr);
		smash.ExecuteCommand(cmdStr.c_str());
	}

	return 0;
}