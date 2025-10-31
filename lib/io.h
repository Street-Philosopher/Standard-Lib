
#pragma once

#include "common.h"

#include "files.h"

#define STDIN_FILEN  0
#define STDOUT_FILEN 1

void write(int fd, char* buf, u64 len);
void read (int fd, char* buf, u64 len);

void ioctl(int fd, int IOCTL_number, void* termios);

#define newl() print("\n", 1)
void print(char* buf, u64 len);
void print2(char* string);

#define IOCTL_TCGETS 0x5401
#define IOCTL_TCSETS 0x5402
char getc();
char getc_mute();
void input(char* buf, u64 len);

void exit(int code);

void printint(int value);
void printbyte(u8 byte);
