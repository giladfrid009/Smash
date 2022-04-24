#ifndef REDIRECT_H
#define REDIRECT_H

#include "Commands.h"

class RedirectWriteCommand : public InternalCommand
{
	protected:

	std::string command;
	std::string output;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	RedirectWriteCommand(const std::string& cmdStr, const std::string& command, const std::string& output);

	virtual ~RedirectWriteCommand() override = default;

	void Execute() override;
};

class RedirectAppendCommand : public InternalCommand
{
	protected:

	std::string command;
	std::string output;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	RedirectAppendCommand(const std::string& cmdStr, const std::string& command, const std::string& output);

	virtual ~RedirectAppendCommand() override = default;

	void Execute() override;
};

class PipeOutCommand : public InternalCommand
{
	protected:

	std::string left;
	std::string right;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	PipeOutCommand(const std::string& cmdStr, const std::string& left, const std::string& right);

	virtual ~PipeOutCommand() override = default;

	void Execute() override;
};

class PipeErrCommand : public InternalCommand
{
	protected:

	std::string left;
	std::string right;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	PipeErrCommand(const std::string& cmdStr, const std::string& left, const std::string& right);

	virtual ~PipeErrCommand() override = default;

	void Execute() override;
};

class TouchCommand : public InternalCommand
{
	protected:

	std::string path;
	time_t time;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	TouchCommand(const std::string& cmdStr, const std::string& path, time_t time);

	virtual ~TouchCommand() override = default;

	void Execute() override;
};

class TailCommand : public InternalCommand
{
	protected:

	std::string path;
	int count;

	public:

	static Command* Create(const std::string& cmdStr, const std::vector<std::string>& cmdArgs);

	TailCommand(const std::string& cmdStr, const std::string& path, int count = 10);

	virtual ~TailCommand() override = default;

	void Execute() override;
};

#endif // !REDIRECT_H
