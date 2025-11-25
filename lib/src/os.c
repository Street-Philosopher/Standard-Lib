
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
	
	// make the child execute our commands
	int child = execve(cmd, argv, envp);
	// TODO: this is not reached (???)
	printint(child); newl();
	
	// wait for the child to die
	int retc = wait4(child, 0, 0, 0);

	return (retc);

	// unreachable
}

int system(char* command) {

	char* comd;
	char** argv;
	char** envp;

	argv = splitstr(command, ' ');
	comd = argv[0];
	envp = (char**){ nullptr };

	return cmd(comd, argv, envp);
}
