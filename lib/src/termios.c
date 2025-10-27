
#include "../common.h"

#include "../io.h"
#include "../calls.h"
#include "../termios.h"

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
