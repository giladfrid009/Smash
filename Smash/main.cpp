#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <signal.h>
#include "Smash.h"
#include "SigHandlers.h"
#include "Parser.h"

using std::string;

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

	/*string command = "cd ..; cd ..; ls -a;  ";

	bool isBG = IsRunInBackground(command);
	std::string x = RemoveBackgroundSign(command);

	SpecialCommands cmdType = GetSpecialCommand(command);

	auto res = ParseCommand(command);

	CallExternal("pwd; cd ..; cd ..; ls -a;");*/

	Smash& smash = Smash::GetInstance();

	while (true)
	{
		std::cout << smash.GetPrompt();
		std::string cmdStr;
		std::getline(std::cin, cmdStr);
		smash.ExecuteCommand(cmdStr);
	}

	return 0;
}
