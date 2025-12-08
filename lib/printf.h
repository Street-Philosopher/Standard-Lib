#pragma once

#define FORMAT_STR 		"%s"
#define FORMAT_INT 		"%d"
#define FORMAT_FLOAT 	"%f"

/*
 * % sign indicates format (write %% for writing a single "%" sign)
 * d indicates integer. specify the size in bits after it (or empty for default = sizeof(int) bytes) and add an u for unsigned
 * f indicates float, fd for double
 * s indicates string
 * 
 * returns the number of characters written to `stdout`
 */
int printf(char* format, ...);