
#pragma once

//  register order for syscall: rax (number), rdi, rsi, rdx, r10, r8, and r9
#define SYSCALL_ASM_CALL(num) asm("mov rax, %0\n\tsyscall" :: "i"(num));

#define SYSCALL_READ		0x00
#define SYSCALL_WRITE		0x01
#define SYSCALL_OPEN		0x02
#define SYSCALL_CLOSE		0x03
#define SYSCALL_LSEEK		0x08
#define SYSCALL_IOCTL		0x10
#define SYSCALL_GETPID		0x27
#define SYSCALL_FORK		0x39
#define SYSCALL_VFORK		0x3A
#define SYSCALL_EXECVE		0x3B
#define SYSCALL_EXIT		0x3C
#define SYSCALL_WAIT4		0x3D
#define SUSCALL_KILL		0x3E		// ඞ 