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

int main() {

	malloc(3);

	int x = printf("sees %fen ciao negher %faf sium\n", 1,3,2,5,5,67,7,3,7,8,3,1,67,6,4,2,6,5,3,2,2,5);
	printint(x);
}
