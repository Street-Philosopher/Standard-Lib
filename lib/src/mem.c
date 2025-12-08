#include "../mem.h"
#include "../calls.h"

SYSCALL_DECLARE(SYSCALL_BRK, brk /*, (u64 brk)*/);

u64 brkto(u64 addr) {
	u64 rv;
	if (curbrk() == (rv = brk(addr))) {
		return -1;
	}

	return rv;
}
u64 sbrk(u64 by) {
	return brkto(curbrk() + by);
}

/* 
 * `addr`:	address we suggest the OS should put our memory at
 * `len`:	length of the mapping (see `offset`)
 * `prot`:	area protections (read, write, etc)
 * `flags`:	exactly one of the flags defined in `mem.h`
 * `fd`:	associated file (see `offset`)
 * `off`:	The contents of a file mapping are initialized using `length` bytes starting at offset `offset` in the file descriptor `fd`. must be a multiple of the page size
 */
void* mmap(u64 addr, u64 len, u64 prot, u64 flags, u64 fd, u64 off) {
	asm volatile("mov r10, rcx");		// i've always been so wrong
	SYSCALL_ASM_CALL(SYSCALL_MMAP)
}
SYSCALL_DECLARE(SYSCALL_MUNMAP, munmap)

// are these bad? yes.
// do i care? also yes.
// should i change them? probably.
// but does this teach me more about the cpu architecture? ok listen
// i mean it was an excuse to learn a bit more on branch prediction, more in depth archtecture stuff etc
// but that's just bc of how terrible this code is
// asm(
// 	".globl	memcpy					\n\t"
// 	".type	memcpy, @function		\n"

// 	"memcpy_fr:						\n\t"
// 		"jmp .check3				\n"
// 	".lab3:							\n\t"
// 		"mov al, BYTE PTR[rdi]		\n\t"
// 		"mov BYTE PTR [rsi], al		\n\t"
// 		"add rdi, 1					\n\t"
// 		"add rsi, 1					\n\t"
// 		"sub rdx, 1					\n"
// 	"memcpy:						\n"
// 	".check3:						\n\t"
// 		"test rdx,rdx				\n\t"
// 		"jne .lab3					\n\t"
// 		"ret						\n\t"

// 	".size	memcpy, .-memcpy_fr"
// );
void memcpy(void* from, void* to, u64 size) {
	while(size--) {
		((char*)to)[size] = ((char*)from)[size];
	}
}

void memmov(void* from, void* to, u64 size);

// asm(
// 	".globl	memset					\n\t"
// 	".type	memset, @function		\n"

// 	"memset:						\n\t"
// 		"mov rax,rsi				\n\t"
// 		"jmp .check					\n"
// 	".lab1:							\n\t"
// 		"mov BYTE PTR [rdi], al		\n\t"
// 		"add rdi, 1					\n\t"
// 		"sub rdx, 1					\n"
// 	".check:						\n\t"
// 		"test rdx,rdx				\n\t"
// 		"jne .lab1					\n\t"
// 		"ret						\n\t"

// 	".size	memset, .-memset"
// );
void memset(void* ptr, u64 value, u64 size) {
	while(size--) {
		((char*)ptr)[size] = value;
	}
}

// asm(
// 	".globl	memcmp					\n\t"
// 	".type	memcmp, @function		\n"

// 	"memcmp_fr:						\n\t"
// 		"jmp .check2				\n"
// 	".lab2:							\n\t"
// 		"mov al, BYTE PTR [rdi]		\n\t"
// 		"cmp al, BYTE PTR [rsi]		\n\t"
// 		"jne .nope					\n\t"
// 		"add rdi, 1					\n\t"
// 		"add rsi, 1					\n\t"
// 		"sub rdx, 1					\n"
// 	"memcmp:						\n"
// 	".check2:						\n\t"
// 		"test rdx,rdx				\n\t"
// 		"jne .lab2					\n\t"
// 		"mov eax, 1					\n\t"
// 		"ret						\n"
// 	".nope:							\n\t"
// 		"xor eax, eax				\n\t"
// 		"ret						\n\t"

// 	".size	memcmp, .-memcmp_fr"
// );
bool memcmp(void* pt1, void* pt2, u64 size) {
	while(size--) {
		if (((char*)pt1)[size] != ((char*)pt2)[size])
			return false;
	}
	return true;
}
