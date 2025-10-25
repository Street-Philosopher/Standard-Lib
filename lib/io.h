
#pragma once

#include "common.h"

#define STDIN_FILEN  0
#define STDOUT_FILEN 1

#define newline() print("\n", 1)
#define print2(x) print(x, sizeof(x))
void print(char* text, u64 length);
void print_nolen(char* text);

#define IOCTL_TCGETS 0x5401
#define IOCTL_TCSETS 0x5402
char getc();
void input(char* outbuf, u64 bufsize);

void exit(int code);
