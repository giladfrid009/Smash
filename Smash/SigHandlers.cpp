#include "SigHandlers.h"
#include <iostream>

void Handler_CtrlZ(int sigNum)
{
	std::cout << "ctrl+z\n";
}

void Handler_CtrlC(int sigNum)
{
	std::cout << "ctrl+c\n";
}

void Handler_Alarm(int sigNum)
{
	std::cout << "alarm\n";
}