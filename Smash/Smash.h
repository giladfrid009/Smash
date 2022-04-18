#ifndef SMASH_H
#define SMASH_H

#include "Commands.h"
#include "JobList.h"
#include <vector>
#include <string>

class Smash
{
	private:
	JobsList jobs;
	pid_t currentPid;
	std::string promptText;

	Smash();

	public:

	Smash(Smash const&) = delete;

	void operator=(Smash const&) = delete;

	~Smash();

	static Smash& Instance();

	Command* CreateCommand(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	std::string Prompt();

	pid_t CurrentPid();

	void ExecuteCommand(std::string& cmdStr);

	friend void JobsCommand::Execute();

	friend void KillCommand::Execute();

	friend void BackgroundCommand::Execute();
};

#endif // !SMASH_H

