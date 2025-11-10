#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"

#define size (u64)0x100000000

int main() {
	u8* ptr = malloc(size);
	
	if (ptr == -1) {
		print("😭😭");
		exit(-1);
	}

	for (u64 i = 0; i < size; i++) {
		// print("numba ");
		// printhex(i);
		// print(": ");
		ptr[i] = 'A';
		// print("good\n");
	}

	print("lol");
	getc_mute();
}