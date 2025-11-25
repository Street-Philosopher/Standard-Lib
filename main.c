#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/proc.h"
#include "lib/calls.h"
#include "lib/files.h"
#include "lib/error.h"
#include "lib/common.h"


volatile c = 1;
void printass(int n) {
	print("ass\n");
	printint(n); newl();
	c = 0;
}

int main() {

	int c2 = sigaction(SIGINT, &printass);
	if (c2)
		{ printerr(c2); newl(); }
	else
		print("ok!\n");

	while(c);

	print("lol");
}