#include "lib/io.h"
#include <termios.h>

int _start() {

	char outc[10];

	for (int i = 0; i < 10; i++) {
		outc[i] = getc();
	}

	print_nolen("now going to do the thing\n");

	print(&outc, 10);

	exit(0);
}