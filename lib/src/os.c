
#include "../common.h"
#include "../os.h"

#include "../io.h"
#include "../mem.h"
#include "../util.h"
#include "../proc.h"
#include "../error.h"

// this was so much harder to write than it looks bc i know absolutely nothing abt OSs
int cmd(char* cmd, char** argv, char** envp) {
	
	// make the child execute our commands
	int child = execve(cmd, argv, envp);
	printint(child); newl();
	
	// wait for the child to die
	int retc = wait4(child, 0, 0, 0);

	return (retc);

	// unreachable
}

// TODO: newbuf should be a char**: pointer to an array of char* (strings)
int system(char* command) {

	char* envp[] = { nullptr };
	char* newbuf;

	int rc = 0;

	// cmd_ln now holds length of the whole input string, which means command + params
	int cmd_ln = strln(command);
	newbuf = malloc(cmd_ln);
	if (newbuf == MALLOC_ERR) {
		return -1;
	}

	memcpy(command, newbuf, cmd_ln);

	do {
		if (newbuf[rc] == ' ') newbuf[rc] = 0;
	} while(newbuf[++rc]);

	// cmd_ln now holds the length of the first item (actual command's length)
	cmd_ln = strln(newbuf);

	print(newbuf); newl();
	print(newbuf+cmd_ln); newl();

	// rc = cmd(newbuf, newbuf+cmd_ln, envp);

	free(newbuf);
	return rc;
}
