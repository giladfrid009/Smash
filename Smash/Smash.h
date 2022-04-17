#ifndef SMASH_H
#define SMASH_H

#include "Commands.h"
#include <vector>
#include <string>

class Smash
{
	private:
	std::string promptText = "smash";

	Smash();

	public:

	Smash(Smash const&) = delete;

	void operator=(Smash const&) = delete;

	~Smash();

	static Smash& GetInstance()
	{
		static Smash instance;
		return instance;
	}

	Command* CreateCommand(std::string& cmdStr, std::vector<std::string>& cmdArgs);

	std::string GetPrompt();

	void ExecuteCommand(std::string& cmdStr);
};

#endif // !SMASH_H

