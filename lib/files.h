
#include "common.h"

typedef int fd_t;

#define O_RDONLY		   00
#define O_WRONLY		   01
#define O_RDWR			   02
#define O_CREAT			 0100
#define O_APPEND 		02000

#define S_IRWXU  00700 // user (file owner) has read, write, and execute permission
#define S_IRUSR  00400 // user has read permission
#define S_IWUSR  00200 // user has write permission
#define S_IXUSR  00100 // user has execute permission

#define S_IRWXG  00070 // group has read, write, and execute permission
#define S_IRGRP  00040 // group has read permission
#define S_IWGRP  00020 // group has write permission
#define S_IXGRP  00010 // group has execute permission

#define S_IRWXO  00007 // others have read, write, and execute permission
#define S_IROTH  00004 // others have read permission
#define S_IWOTH  00002 // others have write permission
#define S_IXOTH  00001 // others have execute permission

/* Values for the WHENCE argument to lseek.  */
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */
#define SEEK_DATA	3	/* Seek to next data.  */
#define SEEK_HOLE	4	/* Seek to next hole.  */

fd_t open(char* fname, u32 flags, u32 mode);
int close(fd_t fd);
fd_t fopen(char* fname, bool append);
fd_t fcreate(char* fname);
long lseek(fd_t, long, int);

int fstat(fd_t fd, void* out);
int stat (char* fname, void* out);