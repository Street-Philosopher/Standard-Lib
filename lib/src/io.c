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
	// actually it does. my whole life has been a lie. i have an exam in one week and have not studied in the last four days. i have not eaten nor drank in the whole day because of this* function. i just want to see my family
	// * note: the "this" actually refers to the mmap function
	SYSCALL_ASM_CALL(SYSCALL_WRITE)
}
void read (int fd, char* buf, u64 len) {
	SYSCALL_ASM_CALL(SYSCALL_READ)
}

void ioctl(int fd, int IOCTL_number, void* termios) {
	SYSCALL_ASM_CALL(SYSCALL_IOCTL)
}

fd_t open(char* fname, u32 flags, u32 mode) {
	SYSCALL_ASM_CALL(SYSCALL_OPEN)
}
int close(fd_t fd) {
	SYSCALL_ASM_CALL(SYSCALL_CLOSE)
}
long lseek(fd_t fd, long offset, int whence) {
	SYSCALL_ASM_CALL(SYSCALL_LSEEK)
}

int fstat(fd_t fd, void* out) {
	SYSCALL_ASM_CALL(SYSCALL_FSTAT)
}
int stat (char* fname, void* out) {
	SYSCALL_ASM_CALL(SYSCALL_STAT)
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
		"mov rax, %0\n\t"
		"syscall\n\t"
		// if for whatever reason the syscall didn't work (???) we can also
		// exit the program by overflowing the stack
		"jmp exit"
	
		::	"i" (SYSCALL_EXIT)
	);
	// here bc the compiler is annoying
	while(1);
}


void printint(int value) {
	const int max_int_val = 0x7FFFFFFF;
	const int min_int_val = max_int_val+1;		// this is mildly funny
	const int sign_bitmsk = 1 << 31;

	bool print_zero_flag = false;
	int  pow = 1E9, res;
	char tmp;

	// if the sign bit is set print the sign and forget about it
	if (value & sign_bitmsk) print("-", 1);

	do {
		res = value / pow;

		if (res || print_zero_flag) {
			print_zero_flag = true;
			value -= res * pow;
			tmp = '0' + abs(res);		// stupidly optimised function within a stupidly unoptimised function let's fucking go
			print(&tmp, 1);
		}

		pow /= 10;
	} while (pow > 0);

}

char hexdigit(u8 val) { val += (val < 0xA) ? '0' : ('A'-0xA); return val; }

void printbyte(u8 byte) {
	char tmp;
	print("0x", 2);
	// print most significant hex digit
	tmp = byte >> 4;
	if (0 <= tmp && tmp <= 9) {
		tmp += '0';
		print(&tmp, 1);
	} else {
		tmp += 'A' - 0xA;
		print(&tmp, 1);
	}
	// print least significant hex digit
	tmp = byte & 0x0F;
	if (0 <= tmp && tmp <= 9) {
		tmp += '0';
		print(&tmp, 1);
	} else {
		tmp += 'A' - 0xA;
		print(&tmp, 1);
	}
}

void printhex(u64 value) {

	bool print_zero_flag = false;
	u64 cur_digit_mask = 0xF000000000000000;
	int shift_by = 60;
	char tmp;

	print("0x", 2);

	while (shift_by > -1) {
		tmp = (value & cur_digit_mask) >> shift_by;

		if (tmp || print_zero_flag) {
			print_zero_flag = true;
			tmp = hexdigit(tmp);
			print(&tmp, 1);
		}

		cur_digit_mask >>= 4;
		shift_by -= 4;
	} ;
}

