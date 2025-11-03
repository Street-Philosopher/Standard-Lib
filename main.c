#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"

int main() {
	char st1[] = "WE GOT DYNAMIC MEMORY";

	fd_t file = open("fuffa", O_RDWR, 0);
	print2("file descriptor: ");
	printint(file);
	newl();

	char* str = (char*)mmap(NULL, PAGES(16), PROT_READ | PROT_WRITE, MAP_PRIVATE, file, PAGES(0));
	if ((int)str < 0) {
		print2("error code: ");
		printint(-(int)str);
		// return -1;
	}

	print2("addr: ");
	printhex((int)str);
	newl();
	printint((int)str);
	newl();

	for (int c = 0; c < 22; c++) {
		str[c] = st1[c];
	}

	print(str, 0x100);

	// char* cm = "/bin/rm";
	// char* argv[] = {cm, "fuffa", 0};
	// char* envp[] = {0};
	// int i = cmd(cm, argv, envp);
	// print2("cmd retval: ");
	// printint(i);
	// newl();
}