#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/error.h"

int main() {
	int x = system("echo let's see if this wroks");

	newl(); printint(x); newl();
}