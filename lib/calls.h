
#pragma once

//  register order for syscall: rax, rdi, rsi, rdx, r10, r8, and r9

#define SYSCALL_READ		0x00
#define SYSCALL_WRITE		0x01
#define SYSCALL_IOCTL		0x10
#define SUSCALL_EXIT		0x3C
