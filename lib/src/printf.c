
#include "../common.h"
#include "../mem.h"

// i like pain but i'm not stupid

struct formattedstr {
	u64 formatterlength;		// length of the formatter used to generate the string itself, for example '3' for "%dxu" (which is hexadecimal unsigned integer)
	char* string;				// string itself
};

/*
 * this function is as generic as possible: we handle everything at high level bc i can't be bothered writing it in asm
 *
 * `void* rbp` is exactly what you think it is (register rbp)
 * `char* outstr` is where the output string will be placed. before the output string will be placed the length of the formatter used to decode the value, as a u64
 * `char* format` points to the format string we're currently looking to translate
 * `returns` the length of the string written in `outstr`
 */
u64 printf_expand_d/*ong*/(void* rbp, struct formattedstr* outstr, char* format) {
	// take input stack pointer instead of value, take input string array and write directly there, return number of written bytes or -1 on bad format
	// use `rbp` to get the argument (hopefully no segfault) based on the type we infer from `format`, then decode the string and write it in outstr returning its lengtgh
	char* x = malloc(8);
	x[0] = x[1] = x[3] = x[5] = x[6] = 'A';
	x[2] = x[4] = 'a';
	x[7] = 0;
	*outstr = (struct formattedstr) { .formatterlength = 3 + 1, .string = x };
	return 7;
}
