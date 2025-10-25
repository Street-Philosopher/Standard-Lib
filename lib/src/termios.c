
#include "../common.h"

#include "../io.h"
#include "../calls.h"
#include "../termios.h"

// arg1: fd
// arg2: IOCTL number
// arg3: pointer to data structure
void ioctl(int fd, int IOCTL_number, struct termios_t* termios) {
	asm(
		"mov rax,%0		\n\t"
		"syscall		\n\t"
		::	"i"(SYSCALL_IOCTL)
	);
}

void termios_set_bits(u32 mask) {
	struct termios_t termios;
	ioctl(STDIN_FILEN, IOCTL_TCGETS, &termios);
	termios.c_lflag |= mask;
	ioctl(STDIN_FILEN, IOCTL_TCSETS, &termios);
}
void termios_reset_bits(u32 mask) {
	struct termios_t termios;
	ioctl(STDIN_FILEN, IOCTL_TCGETS, &termios);
	termios.c_lflag &= ~mask;
	ioctl(STDIN_FILEN, IOCTL_TCSETS, &termios);
}
