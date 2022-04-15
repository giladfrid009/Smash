#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Smash.h"
#include "SigHandlers.h"

//todo: remove later
void CallExternal(string command)
{
	string formattedCommand = "bash -c \"" + command + "\"";

	pid_t pid = fork();
	int status;

	if (pid < 0)
	{
		perror("can't fork");
	}
	else if (pid == 0)
	{
		system(formattedCommand.c_str());
		exit(0);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		std::cout << "executed: " << command << " sucessfully with status " << status << std::endl;
	}
}

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

	CallExternal("pwd; cd ..; ls -a;");

	while (true)
	{
		std::cout << smash.GetPrompt();
		std::string cmdStr;
		std::getline(std::cin, cmdStr);
		smash.ExecuteCommand(cmdStr.c_str());
	}

	return 0;
}
