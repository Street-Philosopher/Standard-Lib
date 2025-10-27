#include "../io.h"
#include "../util.h"
#include "../calls.h"
#include "../termios.h"

void write(int fd, char* buf, u64 len) {
	// this and pretty much all other assembly code i wrote is probably highly dependant on compiler conventions lol
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
		"mov rax,%0		\n\t"
		"syscall		\n\t"
		::	"i"(SYSCALL_IOCTL)
	);
}

void print(char* buf, u64 len) {
	write(STDOUT_FILEN, buf, len);
	// asm volatile (
	// 	// rdx <- length of string excluding null terminator
	// 	"mov rdx, rsi\n\t"
	//
	// 	// rsi <- addr of msg
	// 	"mov rsi, rdi\n\t"
	//
	// 	// rdi <- file descriptor
	// 	"mov rdi, %1\n\t"
	//
	// 	// rax <- number of syscall
	// 	"mov rax, %0\n\t"
	//
	// 	"syscall\n\t"
	//
	// 	/* outputs. */
	// 	:
	//
	// 	/* inputs */
	// 	:	"i" (SYSCALL_WRITE),
	// 		"i" (STDOUT_FILEN)
	// );
}
void print2(char* string) {
	// same as above but infer length from the string

	write(STDOUT_FILEN, string, strln(string));

}

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
