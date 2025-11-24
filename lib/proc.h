
#pragma once

#include "signals.h"
#include "portability.h"

int getpid();
int fork();
int vfork();
int execve(char* file, char* argv[], char* envp[]);
int wait4(int upid, int* stat_addr, int options, void* rusage);

int kill(int pid, int sig);
int raise(int sig);

int rt_sigaction (int sig, const sigaction_t* act, sigaction_t* old_act);
int sigaction(int sig, sighandler_t handler);
int signalfd();

// typedef void (*sighandler_t)(int);
