#ifndef SMASH_SIGHANDLERS_H
#define SMASH_SIGHANDLERS_H

void Handler_CtrlZ(int sigNum);
void Handler_CtrlC(int sigNum);
void Handler_Alarm(int signalNum, siginfo_t* info, void* context);

#endif //SMASH_SIGHANDLERS_H
