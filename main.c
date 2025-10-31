#include "lib/io.h"
#include "lib/os.h"

int main() {
	char* argv[] = {"/bin/sleep", "2", nullptr};
	char* envp[] = {nullptr};
	system("/bin/sleep", argv, envp);
}