
// To find the implementation of a system call go into kernel and `grep -rA3 "SYSCALL_DEFINE.\?(syscall," *`

#pragma once

#ifndef stringify
#define stringify(x) #x
#endif

//		register order for syscall: rax (number), rdi, rsi, rdx,   r10,   r8, r9
//		register order for fncall: 				  rdi, rsi, rdx, !!RCX!!, r8, r9
#define SYSCALL_ASM_CALL(num) 					asm volatile ("mov rax, %0\n\tsyscall" :: "i"(num));
#define SYSCALL_DECLARE(code, fname)			/* u64 fname args; */ asm (".globl " #fname "\n\t" ".type " #fname ", @function\n" #fname ":\n\t" "mov rax, " stringify(code) "\n\t" "syscall" "\n\t" "ret");
#define SYSCALL_DECLARE_LONG(code, fname)		/* u64 fname args; */ asm (".globl " #fname "\n\t" ".type " #fname ", @function\n" #fname ":\n\t" "mov r10, rcx" "\n\t" "mov rax, " stringify(code) "\n\t" "syscall" "\n\t" "ret");

#define SYSCALL_READ		0x00
#define SYSCALL_WRITE		0x01
#define SYSCALL_OPEN		0x02
#define SYSCALL_CLOSE		0x03
#define SYSCALL_STAT		0x04
#define SYSCALL_FSTAT		0x05
#define SYSCALL_LSEEK		0x08
#define SYSCALL_MMAP		0x09
#define SYSCALL_MUNMAP		0x0B
#define SYSCALL_BRK			0x0C
#define SYSCALL_RT_SIGACT	0x0D
#define SYSCALL_SIGRET		0x0F
#define SYSCALL_IOCTL		0x10
#define SYSCALL_GETPID		0x27
#define SYSCALL_FORK		0x39
#define SYSCALL_VFORK		0x3A
#define SYSCALL_EXECVE		0x3B
#define SYSCALL_EXIT		0x3C
#define SYSCALL_WAIT4		0x3D
#define SUSCALL_KILL		0x3E		// ඞ 