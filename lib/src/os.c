
#include "../common.h"
#include "../os.h"

#include "../io.h"
#include "../proc.h"

int system(char* cmd, char** argv, char** envp) {

	vfork();

	// we're now inside the child and the parent is waiting doing nothing

	// make the child execute our commands
	int retc = (execve(cmd, argv, envp));
	print2("child exit code: ");
	printint(retc);
	newl();

	// once the child has executed your commands, terminate the child
	exit(retc);

	// unreachable
}
