#include "Smash.h"
#include "SigHandlers.h"
#include "Parser.h"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <signal.h>

using std::string;

int main(int argc, char* argv[])
{
	if (signal(SIGTSTP, Handler_CtrlZ) == SIG_ERR)
	{
		perror("smash error: signal failed");
	}

	if (signal(SIGINT, Handler_CtrlC) == SIG_ERR)
	{
		perror("smash error: signal failed");
	}

	struct sigaction sa = {0}; //todo: maybbe remove, move back to old handler
	sa.sa_sigaction = Handler_Alarm;
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGALRM, &sa, nullptr) < 0)
	{
		perror("smash error: sigaction failed");
	}

	Smash& smash = Smash::Instance();

	while (true)
	{
		std::cout << smash.Prompt();
		std::string cmdStr;
		std::getline(std::cin, cmdStr);
		smash.ExecuteCommand(cmdStr);
	}

	return 0;
}

