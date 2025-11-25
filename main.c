#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/proc.h"
#include "lib/calls.h"
#include "lib/files.h"
#include "lib/error.h"
#include "lib/common.h"


// #define _POSIX_C_SOURCE 199309L
// #include <stdio.h>
// #include <string.h>
// #include <signal.h>

// typedef void (*sighandler_t) (int);

// typedef struct wtf_linux {
// 	sighandler_t handler;
// 	unsigned long modified_flags;
// 	void (*restorer)(void);
// 	__sigset_t sa_mask;
// } actual_sigaction_t;

// asm (
// ".globl	restorer"																																																		"\n\t"
// ".type	restorer, @function"																																															"\n"
// "restorer:"																																																				"\n\t"
// 	"mov rax,15"		/* SYSCALL_SIGRET */																																											"\n\t"
// 	"syscall"
// );
// void restorer();

// int rt_sigaction(int sig, const struct sigaction* act, struct sigaction* old_act) {

// 	// i was betrayed by the penguin

// 	actual_sigaction_t bro;
// 	bro.handler = act->sa_handler;
// 	bro.restorer = &restorer;

// 	asm("\n");

// 	bro.modified_flags = ((signed long)act->sa_flags) | (1 << 26);

// 	asm("\n");
	
// 	// copy the whole table thing
// 	bro.sa_mask = act->sa_mask;

// 	asm volatile("\nlea rax, %0\n\tmov rsi, rax" :: "m"(bro) );

// 	// syscall define for rt_sigaction literally starts with
// 	/*
// 		if (sigsetsize != sizeof(sigset_t))
// 			return -EINVAL;
// 	*/
// 	// wtf??
// 	asm volatile("\nmov r10, 0x08");
// 	// guess it makes half sense still wtf

// 	SYSCALL_ASM_CALL(SYSCALL_RT_SIGACT)
// }


volatile c = 1;
void printass(int n) {
	print("ass\n");
	printint(n); newl();
	c = 0;
}

int main() {
	
	const sigaction_t act = {
		.sa_handler = &printass,
		.sa_flags = 0,
		.sa_mask.__val = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
		// .sa_flags = 0xdeadbeef,
		// .sa_mask.__val={
		// 	0x55aa55aa55aa55aa,
		// 	0x1111111111111111,
		// 	0x2222222222222222,
		// 	0x3333333333333333,
		// 	0x4444444444444444,
		// 	0x5555555555555555,
		// 	0x6666666666666666,
		// 	0x7777777777777777,
		// 	0x8888888888888888,
		// 	0x9999999999999999,
		// 	0xaaaaaaaaaaaaaaaa,
		// 	0xbbbbbbbbbbbbbbbb,
		// 	0xcccccccccccccccc,
		// 	0xdddddddddddddddd,
		// 	0xeeeeeeeeeeeeeeee,
		// 	0xffffffffffffffff
		// }
	};
	// printf("act: 0x%08X\nhandler: 0x%08X\n\n", (unsigned long)(&act), (unsigned long)(&printass));

	int c2 = rt_sigaction(SIGINT, &act, NULL);
	if (c2)
		{ printerr(c2); newl(); }
	else
		print("ok!\n");

	while(c);

	print("lol");
}