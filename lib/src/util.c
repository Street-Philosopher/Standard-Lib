
#include "../common.h"

// lol 
asm(
	".globl abs			\n\t"
	"abs:				\n\t"
	"mov eax,edi		\n\t\t"
	"sar edi, 31		\n\t\t"
	"xor eax,edi		\n\t\t"
	"add eax,edi		\n\t\t"
	"ret"
);
u64 strln(char* str) {
	register u64 c = 0;
	while (str[c++]);
	return c;
}
