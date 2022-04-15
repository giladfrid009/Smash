#ifndef SMASH_COMMAND_H
#define SMASH_COMMAND_H

#include <vector>
#include <string.h>

/* TODO: important
* implement foreground Sleep command for testing
* implement backGround Sleep command for testing
* implement unknown Sleep command for testing
* implemet blank command for pipe tests
* test these commands in different scenarios, only after everything is working start adding the "real" commands.
*/
class Command
{
	public:

	Command(const char* cmdStr);

	virtual ~Command();

	virtual void Execute() = 0;
	//virtual void prepare();
	//virtual void cleanup();
};

class BuiltInCommand : public Command
{
	public:

	BuiltInCommand(const char* cmdStr);

	virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command
{
	public:

	ExternalCommand(const char* cmdStr);

	virtual ~ExternalCommand() {}

	void Execute() override;
};

class PipeCommand : public Command
{
	public:

	PipeCommand(const char* cmdStr);

	virtual ~PipeCommand() {}

	void Execute() override;
};

class RedirectionCommand : public Command
{
	public:

	explicit RedirectionCommand(const char* cmdStr);

	virtual ~RedirectionCommand() {}

	void Execute() override;
	//void prepare() override;
	//void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand
{
	ChangeDirCommand(const char* cmdStr, char** plastPwd);

	virtual ~ChangeDirCommand() {}

	void Execute() override;
};

class GetCurrDirCommand : public BuiltInCommand
{
	public:
	GetCurrDirCommand(const char* cmdStr);

	virtual ~GetCurrDirCommand() {}

	void Execute() override;
};

class ShowPidCommand : public BuiltInCommand
{
	public:

	ShowPidCommand(const char* cmdStr);

	virtual ~ShowPidCommand() {}

	void Execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand
{
	QuitCommand(const char* cmdStr, JobsList* jobs);

	virtual ~QuitCommand() {}

	void Execute() override;
};

class JobsList
{
	public:

	class JobEntry
	{
	};

	public:

	JobsList();

	~JobsList();

	void AddJob(Command* cmd, bool isStopped = false);

	void PrintJobsList();

	void KillAllJobs();

	void RemoveFinishedJobs();

	JobEntry* GetJobById(int jobId);

	void RemoveJobById(int jobId);

	JobEntry* GetLastJob(int* lastJobId);

	JobEntry* GetLastStoppedJob(int* jobId);
};

class JobsCommand : public BuiltInCommand
{
	public:

	JobsCommand(const char* cmdStr, JobsList* jobs);

	virtual ~JobsCommand() {}

	void Execute() override;
};

class KillCommand : public BuiltInCommand
{
	public:

	KillCommand(const char* cmdStr, JobsList* jobs);

	virtual ~KillCommand() {}

	void Execute() override;
};

class ForegroundCommand : public BuiltInCommand
{
	public:

	ForegroundCommand(const char* cmdStr, JobsList* jobs);

	virtual ~ForegroundCommand() {}

	void Execute() override;
};

class BackgroundCommand : public BuiltInCommand
{
	public:

	BackgroundCommand(const char* cmdStr, JobsList* jobs);

	virtual ~BackgroundCommand() {}

	void Execute() override;
};

class TailCommand : public BuiltInCommand
{
	public:

	TailCommand(const char* cmdStr);

	virtual ~TailCommand() {}

	void Execute() override;
};

class TouchCommand : public BuiltInCommand
{
	public:

	TouchCommand(const char* cmdStr);

	virtual ~TouchCommand() {}

	void Execute() override;
};

#endif //SMASH_COMMAND_H_
