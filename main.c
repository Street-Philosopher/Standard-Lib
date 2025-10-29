#include "lib/io.h"

int main() {

	fd_t file = fcreate("lolfile.c");
	if (file == -1) {
		print2("error reading file");
		return -1;
	}

	print2("let's fucking go\n");

	write(file, "#include <stdio.h>\nint main() { printf(\"hello lol\"); }", 100);

	close(file);

	print2("d0ne");

	return 0;
}