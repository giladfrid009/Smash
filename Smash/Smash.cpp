#include "Smash.h"
#include "Commands.h"

#include <string.h>

using namespace std;

Smash::Smash()
{
}

Smash::~Smash()
{
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmdStr)
*/
Command* Smash::CreateCommand(const char* cmdStr)
{
	// For example:
	/*
	  string cmd_s = _trim(string(cmdStr));
	  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

	  if (firstWord.compare("pwd") == 0) {
		return new GetCurrDirCommand(cmdStr);
	  }
	  else if (firstWord.compare("showpid") == 0) {
		return new ShowPidCommand(cmdStr);
	  }
	  else if ...
	  .....
	  else {
		return new ExternalCommand(cmdStr);
	  }
	  */
	return nullptr;
}

void Smash::ExecuteCommand(const char* cmdStr)
{
	// for example:
	// Command* cmd = CreateCommand(cmdStr);
	// cmd->execute();
	// Please note that you must fork smash process for some commands (e.g., external commands....)
}