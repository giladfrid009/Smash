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

	if (signal(SIGALRM, Handler_Alarm) == SIG_ERR) //todo: fix later
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
