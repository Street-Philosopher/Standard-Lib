#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/proc.h"
#include "lib/files.h"
#include "lib/error.h"
#include "lib/common.h"

void printass(int n) {
	print("ass\n");
	printint(n); newl();
}

int main() {
	
	const sigaction_t act = {.sa_handler = &printass, .sa_flags = 0};

	printerr(rt_sigaction(SIGINT, &act, NULL)); newl();

	while(1);
}