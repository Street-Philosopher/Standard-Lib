
#pragma once

#include "common.h"

#include "files.h"

#define STDIN_FILEN  0
#define STDOUT_FILEN 1

u64 write(int fd, char* buf, u64 len);
u64 read (int fd, char* buf, u64 len);

u64 ioctl(int fd, int IOCTL_number, void* termios);

#define newl() print2("\n", 1)
void print2(char* buf, u64 len);
void print(char* string);

#define IOCTL_TCGETS 0x5401
#define IOCTL_TCSETS 0x5402
char getc();
char getc_mute();
void input(char* buf, u64 len);

void exit(int code);

void printint(int value);
void printbyte(u8 byte);
void printhex(u64 value);
void printhex_s(s64 value);

#include "printf.h"

// TODO: printf, input for things other than str (i guess scanf)
