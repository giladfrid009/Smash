#ifndef SMASH_H
#define SMASH_H

#include <vector>
#include <string>
#include "Commands.h"

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

	Command* CreateCommand(std::string& cmdStr);

	std::string GetPrompt();

	void ExecuteCommand(std::string& cmdStr);
};

#endif // !SMASH_H

