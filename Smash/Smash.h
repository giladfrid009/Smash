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
	std::string promptText = "smash";

	Smash();

	public:

	Smash(Smash const&) = delete;

	void operator=(Smash const&) = delete;

	~Smash();

	static Smash& Instance()
	{
		static Smash instance;
		return instance;
	}

	Command* CreateCommand(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	std::string Prompt();

	void ExecuteCommand(std::string& cmdStr);
};

#endif // !SMASH_H

