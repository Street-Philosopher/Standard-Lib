#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/str.h"
#include "lib/proc.h"
#include "lib/util.h"
#include "lib/calls.h"
#include "lib/files.h"
#include "lib/error.h"
#include "lib/debug.h"
#include "lib/common.h"

#include <stdarg.h>


int main() {

	debug_msg("strcpy done")

	int x = printf("sees %dxen ciao %dnnn %d\\xsium\n", 0x123456789abcdef0, 0x55aa55aa, 69420);
	printint(x);
}
