#include "lib/io.h"
#include "lib/os.h"

int main() {
	
	print2("hi! waiting for 2s\n");

	char* argv[] = {"sleep", "10", nullptr};
	char* envp[] = {nullptr};
	int i = cmd("sleep", argv, envp);

	print2("nice!\n");
	printint(i);
	newl();

}