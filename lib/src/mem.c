#include "../mem.h"
#include "../calls.h"

u64 brk(u64 brk) {
	SYSCALL_ASM_CALL(SYSCALL_BRK)
}

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

void* mmap(u64 addr, u64 len, u64 prot, u64 flags, u64 fd, u64 off) {
	asm volatile("mov r10, rcx");		// i've always been so wrong
	SYSCALL_ASM_CALL(SYSCALL_MMAP)
}
int munmap(u64 addr, u64 len) {
	SYSCALL_ASM_CALL(SYSCALL_MUNMAP)
}

// this makes it impossible to use `free()`. too bad!
void* malloc(u64 size) {
	void* rv = (void*)curbrk();

	if (sbrk(size) == -1)
		return -1;
	else
		return rv;
}
// void  free(void* ptr) {
//
// }
void* realloc(void* ptr, u64 size) {
	
}

void memcpy(void* from, void* to, u64 size);
void memmov(void* from, void* to, u64 size);
void memset(void* ptr, u64 value, u64 size);
bool memcmp(void* pt1, void* pt2, u64 size);
