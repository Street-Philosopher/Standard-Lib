
#include "../common.h"
#include "../util.h"
#include "../str.h"
#include "../mem.h"

#include "../debug.h"

// i like pain but i'm not stupid

struct formattedstr {
	u64 formatterlength;		// length of the formatter used to generate the string itself, for example '3' for "%dxu" (which is hexadecimal unsigned integer)
	char* string;				// string itself
};
typedef struct {
	unsigned int gp_offset;
	unsigned int fp_offset;
	void *overflow_arg_area;
	void *reg_save_area;
} c_va_list;
c_va_list ap = {0,0,0,0};

#define gp_offset ap.gp_offset
#define fp_offset ap.fp_offset
#define overflow_arg_area ap.overflow_arg_area
#define reg_save_area ap.reg_save_area

#define abort_if_not_int(type) ((void)sizeof(char[1-(int)(10 * (type)0.2)]));

/* only initialise if uninitialised */
/*                        |         */
/*                        v         */
#define va_start(rbp) if (gp_offset == 0) { gp_offset = 8; fp_offset = 0x30; reg_save_area = rbp -176; overflow_arg_area = 0 /* FIXME */; }

// used for all integer types including pointers
#define va_arg_long(rbp, outval)									\
	if (gp_offset <= 0x2F) {										\
		outval = *(typeof(outval) *)(reg_save_area + gp_offset);	\
		gp_offset += 8;												\
	} else {														\
		outval = *(typeof(outval) *)(overflow_arg_area);			\
		overflow_arg_area += 8;										\
	}


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
	
	struct formattedstr outval;		// value we'll write inside outstr
	u64 stringlength;				// return value

	abort_if_not_int(int)

	va_start(rbp)

	// this is the lowest possible formatter length: the percent sign and the generic command
	// set it here so we don't need to do it inside every command
	outval.formatterlength = 1 + 1;
	outval.string = nullptr;

	// ignore the first character, as it's always the % sign
	switch(format[1]) {
		case 's':
			va_arg_long(rbp, outval.string)
			// FIXME: use a strcpy instead, otherwise we're going to try and free the user's string in the best case and a non-freeable memory area in the worst
			stringlength = strln(outval.string);
			break;
		/*  */
		case 'd':
			u64 val,			// value to print
				cntr = 2;		// current position in format string

			// get actual value
			va_arg_long(rbp, val);

			// check for options
			while(true) {
				switch (format[cntr]) {
					// print number as hex
					case 'x':
						outval.formatterlength++;
						outval.string = hexstr64(val);
						break;
					// ignore the next character, don't treat it as an option. increase formatter length to inform outer function to ignore the backslash, and finish (can't have any other option after backslash)
					case '\\':
						outval.formatterlength++;
						// fall through
					// default is any char that is not an option, so just exit the loop
					default:
						goto integer_option_loop_exit;
				}

				cntr++;
			}
			integer_option_loop_exit:

			// if the string hasn't been set by a specific option yet, set it to the default
			if (outval.string == nullptr)
				outval.string = itos(val);
			
			stringlength = strln(outval.string);

			break;
		default:
		formatter_default:
			debug_warn("bad formatter")
			// fuck it
			PANIC();
			return -1;
	}
	

	// char* x = malloc(8);
	
	// outval = (struct formattedstr) { .formatterlength = 3 + 1, .string = x };
	*outstr = outval;
	return stringlength;
}
