
#pragma once

#include "common.h"

#define B  ((u64)1)
#define KB (0x400 *  B)
#define MB (0x400 * KB)
#define GB (0x400 * MB)

// portability fuck yeah!
#define WORDSIZE (8*B)
#define PAGESIZE (4*KB)
#define PAGES(n) (n*PAGESIZE)


/* MAP STUFF */

#define PROT_READ	0x1		/* Page can be read.  */
#define PROT_WRITE	0x2		/* Page can be written.  */
#define PROT_RW		(PROT_READ | PROT_WRITE)
#define PROT_EXEC	0x4		/* Page can be executed.  */
#define PROT_NONE	0x0		/* Page can not be accessed.  */
#define PROT_GROWSDOWN	0x01000000	/* Extend change to start of
					   growsdown vma (mprotect only).  */
#define PROT_GROWSUP	0x02000000	/* Extend change to start of
					   growsup vma (mprotect only).  */

					   /* Sharing types (must choose one and only one of these).  */
#define MAP_SHARED	0x01		/* Share changes.  */
#define MAP_PRIVATE	0x02		/* Changes are private.  */
#define MAP_SHARED_VALIDATE	0x03	/* Share changes and validate
					   extension flags.  */
#define MAP_TYPE	0x0f		/* Mask for type of mapping.  */

/* Other flags.  */
#define MAP_FIXED	0x10		/* Interpret addr exactly.  */
#define MAP_FILE	0
#define MAP_ANONYMOUS	0x20		/* Don't use a file.  */
/* When MAP_HUGETLB is set bits [26:31] encode the log2 of the huge page size.  */
#define MAP_HUGE_SHIFT	26
#define MAP_HUGE_MASK	0x3f


#define ANON_FILE -1
#define MAP_FAILED	((void *) -1)

void* mmap(u64 addr, u64 len, u64 prot, u64 flags, u64 fd, u64 off);
int munmap(u64 addr, u64 len);
u64 brk(u64 brk);
#define curbrk() brk(0)
// int sbrk(u64 incr);


/* MALLOC STUFF */

#define MALLOC_ERR ((void*)(-1))

void* malloc(u64 size);
void  free(void* ptr);
void* realloc(void* ptr, u64 size);

u64 brkto(u64 addr);
u64 sbrk(u64 by);


/* OTHER MEMORY STUFF */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
void memcpy(void* from, void* to, u64 size);
void memmov(void* from, void* to, u64 size);
void memset(void* ptr, u64 value, u64 size);
bool memcmp(void* ptr1, void* ptr2, u64 size);
#pragma GCC diagnostic pop
