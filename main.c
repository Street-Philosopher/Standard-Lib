#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/str.h"
#include "lib/args.h"
#include "lib/proc.h"
#include "lib/util.h"
#include "lib/calls.h"
#include "lib/files.h"
#include "lib/error.h"
#include "lib/debug.h"
#include "lib/common.h"

struct s1 {
	long a, b, c;
};
struct s2 {
	char a, b, c;
};

void f1(int arg1, ...) {
	
	va_list ap;

	va_start(ap, 0);

	printhex(va_arg(ap, int));
}

int main() {
	f1(0, 0xc0ffee);
}
