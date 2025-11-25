
#include "../proc.h"

#include "../mem.h"
#include "../calls.h"
#include "../common.h"

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


/*
 * i have no idea why. the __libc_sigaction creates a new structure before calling sys_rt_sigaction. the syscall DOES NOT WORK WITH THE DOCUMENTED STRUCTURE
 * WHAT THE FUCK LINUS
 */
typedef struct wtf_linux {
	sighandler_t handler;
	u64 modified_flags;
	void (*restorer)(void);
	__sigset_t sa_mask;
} actual_sigaction_t;

/*
wtf gcc?

lib/src/proc.c:220:18: error: expected ‘)’ before ‘::’ token
  220 |         "syscall"
      |                  ^
      |                  )
  221 |         :: "i"(SYSCALL_SIGRET)
      |         ~~        
lib/src/proc.c:216:5: note: to match this ‘(’
  216 | asm (
      |     ^
*/
asm (
".globl	restorer"																																																		"\n\t"
".type	restorer, @function"																																															"\n"
"restorer:"																																																				"\n\t"
	"mov rax,15"		/* SYSCALL_SIGRET */																																											"\n\t"
	"syscall"																																																			// "\n\t"
);
void restorer();

int rt_sigaction(int sig, const sigaction_t* act, sigaction_t* old_act) {

	// i was betrayed by the penguin

	actual_sigaction_t bro;
	bro.handler = act->sa_handler;
	bro.restorer = &restorer;

	// spacing for easier reading of the assembly files
	asm("");

	bro.modified_flags = ((signed long)act->sa_flags) | (1 << 26);

	asm("");
	
	// copy the whole table thing
	bro.sa_mask = act->sa_mask;

	asm volatile("\n"
		"lea rax, %0"						"\n\t"
		"mov rsi, rax"						"\n\t"
		// TODO:
		:: "m"(bro)
	);

	// syscall define for rt_sigaction literally starts with
	/*
		if (sigsetsize != sizeof(sigset_t))
			return -EINVAL;
	*/
	// wtf??
	asm volatile("\nmov r10, 0x08");
	// probably planning to do something more with it idk

	SYSCALL_ASM_CALL(SYSCALL_RT_SIGACT)

	// FIXME: oldact
}
int sigaction(int sig, sighandler_t handler) {

	sigaction_t act;
	
	act.sa_handler = handler;
	act.sa_flags = 0;
	act.sa_mask = (__sigset_t) { .__val={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } };

	return rt_sigaction(sig, &act, NULL);
}

int raise(int sig) {
	kill(getpid(), sig);
}
