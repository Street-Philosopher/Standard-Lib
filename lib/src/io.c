#include "../io.h"
#include "../calls.h"
#include "../termios.h"

int strln(char* str) {
	register u64 c = 0;
	while (str[c++]);
	return c;
}

void print(char* string, u64 length) {
	// this and pretty much all other assembly code i wrote is probably highly dependant on compiler conventions lol
	asm volatile (
		// rdx <- length of string excluding null terminator
		"mov rdx, rsi\n\t"
		
		// rsi <- addr of msg
		"mov rsi, rdi\n\t"
		
		// rdi <- file descriptor
		"mov rdi, %1\n\t"
		
		// rax <- number of syscall
		"mov rax, %0\n\t"
		
		"syscall\n\t"
		
		/* outputs. */
		:

		/* inputs */
		:	"i" (SYSCALL_WRITE),
			"i" (STDOUT_FILEN)
	);
}
void print_nolen(char* string) {
	// same as above but infer length from the string

	print(string, strln(string));

}

char getc() {

	// bit 1: ICANON (canonical mode on/off)
	// bit 3: ECHO   (print back chars on/off)
	#if ALWAYS_PRINT_BACK_CHARS
	#define TERMIOS_ICANON_BITMASK 0b0010
	#else
	#define TERMIOS_ICANON_BITMASK 0b1010
	#endif

	// first, reset required bits of c_lflag inside the termios structure
	termios_reset_bits(TERMIOS_ICANON_BITMASK);

	// read a single char
	char c;
	input(&c, 1);

	// finally set the bits back to what they once were
	termios_set_bits(TERMIOS_ICANON_BITMASK);

	// return
	return c;

	#undef TERMIOS_ICANON_BITMASK

}
void input(char* outbuf, u64 bufsize) {

	asm volatile (
		// addr of out buffer
		"mov rax, %3\n\t"
		"mov rsi, rax\n\t"
		
		// length of string to be read
		"mov rdx, %2\n\t"
		
		// file descriptor
		"mov rdi, %1\n\t"
		
		// syscall
		"mov rax, %0\n\t"
		
		"syscall\n\t"
	
		:
		
		:	"i" (SYSCALL_READ),
			"i" (STDIN_FILEN),
			"m" (bufsize),
			"m" (outbuf)
	);

	return;

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
}
