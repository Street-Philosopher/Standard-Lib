
#include "../common.h"
#include "../os.h"

#include "../io.h"
#include "../mem.h"
#include "../str.h"
#include "../util.h"
#include "../proc.h"
#include "../error.h"

// this was so much harder to write than it looks bc i know absolutely nothing abt OSs
int cmd(char* cmd, char** argv, char** envp) {
	
	// create a child
	int child_id = fork();
	int status;

	if (child_id == 0) {
		// make the child execute our commands
		execve(cmd, argv, envp);
		print("WHAT\n");

	} else {
		// wait for the child to die
		wait4(child_id, &status, 0, 0);
	}

	return RET_CODE(status);

	// unreachable
}

/*
 * this gets rid of repeated spaces, keep in mind
 */
int system(char* command) {

	char* comd;
	char** argv;
	char** envp;

	argv = splitstr(command, ' ');
	comd = argv[0];
	envp = (char**){ nullptr };

	return cmd(comd, argv, envp);
}
