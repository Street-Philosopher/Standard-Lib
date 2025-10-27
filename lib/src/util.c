
#include "../common.h"

int strln(char* str) {
	register u64 c = 0;
	while (str[c++]);
	return c;
}
