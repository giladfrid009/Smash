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
	std::string prompt;

	Smash();

	public:

	static Smash& Instance();

	Smash(Smash const&) = delete;

	void operator=(Smash const&) = delete;

	~Smash();

	std::string Prompt() const;

	pid_t CurrentPid() const;

	void ExecuteCommand(std::string& cmdStr);

	friend void JobsCommand::Execute();

	friend void KillCommand::Execute();

	friend void BackgroundCommand::Execute();

	friend void ForegroundCommand::Execute();

	friend void ChangePromptCommand::Execute();

	friend void QuitCommand::Execute();

	private:

	Command* CreateCommand(std::string& cmdStr, std::vector<std::string>& cmdArgs) const;
};

#endif // !SMASH_H

