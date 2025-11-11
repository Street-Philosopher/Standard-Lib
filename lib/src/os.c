
#include "../common.h"
#include "../os.h"

#include "../io.h"
#include "../proc.h"

// this was so much harder to write than it looks bc i know absolutely nothing abt OSs
int cmd(char* cmd, char** argv, char** envp) {
	
	// make the child execute our commands
	int child = execve(cmd, argv, envp);
	
	// wait for the child to die
	int retc = wait4(child, 0, 0, 0);

	return (retc);

	// unreachable
}
