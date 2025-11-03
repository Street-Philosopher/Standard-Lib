#include "../mem.h"
#include "../calls.h"

u64 curbrk;

void* mmap(u64 addr, u64 len, u64 prot, u64 flags, u64 fd, u64 off) {
	asm volatile("mov r10, rcx");		// i've always been so wrong
	SYSCALL_ASM_CALL(SYSCALL_MMAP)
}
int munmap(u64 addr, u64 len) {
	SYSCALL_ASM_CALL(SYSCALL_MUNMAP)
}

void* malloc(u64 size) {

}
void  free(void* ptr) {

}
void* realloc(void* ptr, u64 size) {
	
}

void memcpy(void* from, void* to, u64 size);
void memmov(void* from, void* to, u64 size);
void memset(void* ptr, u64 value, u64 size);
bool memcmp(void* ptr1, void* ptr2, u64 size);
