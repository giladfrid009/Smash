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

	Command* CreateCommand(const char* cmdStr);

	Smash(Smash const&) = delete;

	void operator=(Smash const&) = delete;

	static Smash& GetInstance()
	{
		static Smash instance;
		return instance;
	}

	~Smash();

	std::string GetPrompt()
	{
		return promptText + "> ";
	}

	void ExecuteCommand(const char* cmdStr);
};

#endif // !SMASH_H_

