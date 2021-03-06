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
	pid_t selfPID;
	pid_t runningPID;
	std::string prompt;
	std::string prevPath;

	Smash();

	public:

	static Smash& Instance();

	Smash(Smash const&) = delete;

	void operator=(Smash const&) = delete;

	~Smash();

	std::string Prompt() const;

	pid_t RunningPID() const;

	pid_t SelfPID() const;

	void ExecuteRemote(const std::string& cmdStr);

	void Execute(const std::string& cmdStr);

	friend void JobsCommand::Execute();

	friend void KillCommand::Execute();

	friend void BackgroundCommand::Execute();

	friend void ForegroundCommand::Execute();

	friend void ChangePromptCommand::Execute();

	friend void QuitCommand::Execute();

	friend void ChangeDirCommand::Execute();

	private:

	Command* CreateCommand(const std::string& cmdStr, const std::vector<std::string>& cmdArgs) const;
};

#endif // !SMASH_H

