
// global debug enable
// #define DEBUG

#ifdef DEBUG
#
#include "io.h"
#include "color.h"
#
#define warn_not_implemented(x)	do {print(color(FORE_RED) "!!WARNING!!" color(FORE_WHITE) ": `" x "` not implemented\n");} while(0);
#
#define debug_0()				do {print(color(FORE_BLUE) /* color(BACK_WHITE) */ "inside "); print((char*)__func__); print(", line "); printint(__LINE__); print(color(COLOR_RESET_ALL) ": ");} while(0);
#define debug_msg(x)			do {debug_0(); print(x "\n");} while(0);
#define debug_msg_int(i)		do {debug_0(); printint((u64)i); newl();} while(0);
#define debug_msg_addr(addr)	do {debug_0(); printhex((u64)addr); newl();} while(0);
#
#else
// same definitions so we don't get name not defined error
#define warn_not_implemented(x)
#define debug_0()
#define debug_msg(x)
#define debug_msg_int(x)
#define debug_msg_addr(addr)
#endif
