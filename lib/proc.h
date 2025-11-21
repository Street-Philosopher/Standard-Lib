
int getpid();
int fork();
int vfork();
int execve(char* file, char* argv[], char* envp[]);
int wait4(int upid, int* stat_addr, int options, void* rusage);
int kill(int pid, int sig);
//TODO: raise() or kill(), signal(), sigaction()