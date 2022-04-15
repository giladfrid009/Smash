#ifndef SMASH_H
#define SMASH_H

#include <vector>
#include <string.h>
#include <iostream>
#include "Commands.h"

using namespace std;

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

	Command* CreateCommand(const char* cmdStr);

	std::string GetPrompt()
	{
		return promptText + "> ";
	}

	void ExecuteCommand(const char* cmdStr);
};

#endif // !SMASH_H

