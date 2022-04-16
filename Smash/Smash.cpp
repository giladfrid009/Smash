#include "Smash.h"
#include "Commands.h"
#include "Parser.h"

#include <vector>
#include <string>

using std::string;
using std::vector;

Smash::Smash()
{
}

Smash::~Smash()
{
}

Command* Smash::CreateCommand(string& cmdStr)
{
	Commands cmdKind = GetCommand(cmdStr);

	vector<string> cmdArgs = ParseCommand(cmdStr);

	switch (cmdKind)
	{
		case (Commands::Unknown):
		{
			return ExternalCommand::Create(cmdStr, cmdArgs);
		}
		case (Commands::SleepPrint):
		{
			return SleepPrintCommand::Create(cmdStr, cmdArgs);
		}
		default:
		{
			return nullptr;
		}
	}

	return nullptr;
}

void Smash::ExecuteCommand(string& cmdStr)
{
	Command* cmd = CreateCommand(cmdStr);

	if (cmd == nullptr)
	{
		return;
	}

	cmd->Execute();

	delete cmd;
}