
#pragma once

/* idk */
#define NULL 0
#define nullptr ((void*)NULL)

#define PANIC() asm("ud2")


/* mem stuff */
#define B  ((u64)1)
#define KB (0x400 *  B)
#define MB (0x400 * KB)
#define GB (0x400 * MB)

// portability fuck yeah!
#define WORDBIT  (3)
#define WORDSIZE (1 << WORDBIT) // (8*B)
#define PAGESIZE (4*KB)
#define PAGES(n) (n*PAGESIZE)


/* types */
#define true  ((bool)1)
#define false ((bool)0)
typedef int bool;

typedef signed long  s64;
typedef signed int   s32;
typedef signed short s16;
typedef signed char  s8;

typedef unsigned long  u64;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;


/* constants */
#define u64_max ((u64)0xFFFFFFFFFFFFFFFF)
#define u32_max ((u32)0xFFFFFFFF)
#define u16_max ((u16)0xFFFF)
#define u8_max   ((u8)0xFF)

#define s64_max ((s64)0x7FFFFFFFFFFFFFFF)
#define s32_max ((s32)0x7FFFFFFF)
#define s16_max ((s16)0x7FFF)
#define s8_max   ((s8)0x7F)
#define s64_min ((s64)0x8000000000000000)
#define s32_min ((s32)0x80000000)
#define s16_min ((s16)0x8000)
#define s8_min   ((s8)0x80)
