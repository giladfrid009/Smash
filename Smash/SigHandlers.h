#ifndef SMASH_SIGHANDLERS_H
#define SMASH_SIGHANDLERS_H

void Handler_CtrlZ(int sigNum);
void Handler_CtrlC(int sigNum);
void Handler_Alarm(int signalNum);

#endif //SMASH_SIGHANDLERS_H
