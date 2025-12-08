
#include "../common.h"

// i like pain but i'm not stupid

/*
 * this function is as generic as possible: we handle everything at high level bc i can't be bothered writing it in asm
 *
 * `void* rbp` is exactly what you think it is (register rbp)
 * `char* outstr` is where the output string will be placed
 * `char* format` points to the format string we're currently looking to translate
 * `returns` the length of the string written in `outstr`
 */
u64 printf_expand_d/*ong*/(void* rbp, char* outstr, char* format) {
	// take input stack pointer instead of value, take input string array and write directly there, return number of written bytes or -1 on bad format
	// use `rbp` to get the argument (hopefully no segfault) based on the type we infer from `format`, then decode the string and write it in outstr returning its lengtgh
}
