
#include "../common.h"
#include "../calls.h"

int getpid() {
	SYSCALL_ASM_CALL(SYSCALL_GETPID)
}

int fork() {
	SYSCALL_ASM_CALL(SYSCALL_FORK)
}

int vfork() {
	SYSCALL_ASM_CALL(SYSCALL_VFORK)
}

int execve(char* file, char* argv[], char* envp[]) {
	SYSCALL_ASM_CALL(SYSCALL_EXECVE)
}

int wait4(int upid, int* stat_addr, int options, void* rusage) {
	SYSCALL_ASM_CALL(SYSCALL_WAIT4)
}

int kill(int pid, int sig) {
	SYSCALL_ASM_CALL(SUSCALL_KILL)
}
