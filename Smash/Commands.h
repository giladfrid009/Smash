#ifndef SMASH_COMMAND_H
#define SMASH_COMMAND_H

#include <vector>
#include <string>
#include <sys/wait.h>

class Command
{
	protected:

	std::string cmdStr;

	Command(const std::string& cmdStr);

	public:

	virtual ~Command() = default;

	virtual void Execute() = 0;

	std::string ToString();
};

class ExternalCommand : public Command
{
	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	ExternalCommand(const std::string& cmdStr);

	virtual ~ExternalCommand() = default;

	void Execute() override;
};

class InternalCommand : public Command
{
	public:

	InternalCommand(const std::string& cmdStr);

	virtual ~InternalCommand() override = default;
};

class JobsCommand : public InternalCommand
{
	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	JobsCommand(const std::string& cmdStr);

	virtual ~JobsCommand() override = default;

	void Execute() override;
};

class KillCommand : public InternalCommand
{
	protected:

	int signalNum;
	int jobID;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	KillCommand(const std::string& cmdStr, int signalNum, int jobID);

	virtual ~KillCommand() override = default;

	void Execute() override;
};

class BackgroundCommand :public InternalCommand
{
	protected:

	int jobID;
	bool useID;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	BackgroundCommand(const std::string& cmdStr);

	BackgroundCommand(const std::string& cmdStr, int jobID);

	virtual ~BackgroundCommand() override = default;

	void Execute() override;
};

class ForegroundCommand :public InternalCommand
{
	public:

	int jobID;
	bool useID;

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	ForegroundCommand(const std::string& cmdStr);

	ForegroundCommand(const std::string& cmdStr, int jobID);

	virtual ~ForegroundCommand() override = default;

	void Execute() override;
};

class ChangePromptCommand :public InternalCommand
{
	protected:

	std::string prompt;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	ChangePromptCommand(const std::string& cmdStr, std::string prompt);

	virtual ~ChangePromptCommand() override = default;

	void Execute() override;
};

class ShowPidCommand :public InternalCommand
{
	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	ShowPidCommand(const std::string& cmdStr);

	virtual ~ShowPidCommand() override = default;

	void Execute() override;
};

class QuitCommand :public InternalCommand
{
	protected:

	bool killChildren;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	QuitCommand(const std::string& cmdStr, bool killChildren);

	virtual ~QuitCommand() override = default;

	void Execute() override;
};

class RedirectWriteCommand :public InternalCommand //todo:check if special commands 
{
	public:

	std::string command;
	std::string output;

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	RedirectWriteCommand(const std::string& cmdStr, const std::string& command, const std::string& output);

	virtual ~RedirectWriteCommand() override = default;

	void Execute() override;
};

#endif //SMASH_COMMAND_H
