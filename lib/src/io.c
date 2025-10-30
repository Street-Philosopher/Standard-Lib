#include "../io.h"
#include "../util.h"
#include "../calls.h"
#include "../termios.h"

////////////////////////////////////////////
///										 ///
///				 PRIMITIVES				 ///
///										 ///
////////////////////////////////////////////

void write(int fd, char* buf, u64 len) {
	// this and pretty much all other assembly code i wrote is probably highly dependant on compiler conventions lol
	// 🤓☝️ not really	
	asm volatile (
		"mov rax, %0		\n\t"	
		"syscall			\n\t"
	
		::	"i" (SYSCALL_WRITE)
	);
}
void read (int fd, char* buf, u64 len) {
	asm volatile (
		"mov rax, %0		\n\t"
		"syscall"

		:: "i"(SYSCALL_READ)
	);
}

// arg1: fd
// arg2: IOCTL number
// arg3: pointer to data structure
void ioctl(int fd, int IOCTL_number, void* termios) {
	asm(
		"mov rax,%0			\n\t"
		"syscall			\n\t"
		::	"i"(SYSCALL_IOCTL)
	);
}

fd_t open(char* fname, u32 flags, u32 mode) {
	asm(
		"mov rax, %0		\n\t"
		"syscall			\n\t"
		:: "i"(SYSCALL_OPEN)
	);
}
int close(fd_t fd) {
	asm(
		"mov rax, %0		\n\t"
		"syscall			\n\t"
		:: "i"(SYSCALL_CLOSE)
	);
}
long lseek(fd_t fd, long offset, int whence) {
	asm(
		"mov rax, %0		\n\t"
		"syscall			\n\t"
		:: "i"(SYSCALL_LSEEK)
	);
}

////////////////////////////////////////////
///										 ///
///				 COMPOSITES				 ///
///										 ///
////////////////////////////////////////////

/// outputs

void print(char* buf, u64 len) {
	write(STDOUT_FILEN, buf, len);
}
void print2(char* string) {
	// same as above but infer length from the string

	write(STDOUT_FILEN, string, strln(string));

}

/// inputs

// the functions are a copypaste except for the mask
char getc() {
	
	#define TERMIOS_ICANON_BITMASK 0b0010

	// first, reset required bits of c_lflag inside the termios structure
	termios_reset_bits(TERMIOS_ICANON_BITMASK);

	// read a single char
	char c;
	read(STDIN_FILEN, &c, 1);

	// finally set the bits back to what they once were
	termios_set_bits(TERMIOS_ICANON_BITMASK);

	// return
	return c;

	#undef TERMIOS_ICANON_BITMASK

}
char getc_mute() {

	// bit 1: ICANON (canonical mode on/off)
	// bit 3: ECHO   (print back chars on/off)
	#define TERMIOS_ICANON_BITMASK 0b1010

	// first, reset required bits of c_lflag inside the termios structure
	termios_reset_bits(TERMIOS_ICANON_BITMASK);

	// read a single char
	char c;
	read(STDIN_FILEN, &c, 1);

	// finally set the bits back to what they once were
	termios_set_bits(TERMIOS_ICANON_BITMASK);

	// return
	return c;

	#undef TERMIOS_ICANON_BITMASK

}

void input(char* buf, u64 len) {

	read(STDIN_FILEN, buf, len);
}

/// file operations

fd_t fcreate(char* fname) {
	return open(fname, O_RDWR + O_CREAT, S_IRWXU + S_IRGRP + S_IROTH);
}
fd_t fopen(char* fname, bool append) {
	return open(fname, O_RDWR + O_APPEND * append, 0);
}

/// exit

void exit(int code) {
	asm volatile (
		// "mov rdi, code\n\t"		// first param is already in rdi
		"mov rax, %0\n\t"
		"syscall\n\t"
		// if for whatever reason the syscall didn't work (???) we can also
		// exit the program by overflowing the stack
		"jmp exit"
	
		::	"i" (SUSCALL_EXIT)
	);
	// here bc the compiler is annoying
	while(1);
}
