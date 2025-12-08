
#pragma once

#error "not implemented lol"

#include "common.h"

#define c_va_start() asm volatile ("call c_va_start")

u64 c_va_arg_int();
u64 c_va_arg_flt();
u64 c_va_arg_big();

#define c_va_end() asm volatile ("call c_va_end")

/*	MACRO VA_START(list, arg1)
│           0x004018f3      c78530ffff..   mov dword [gp_offset], 8
│           0x004018fd      c78534ffff..   mov dword [fp_offset], 0x30
|
│           0x00401907      488d4510       lea rax, [arg_10h]					; rbp + 0x10
│           0x0040190b      48898538ff..   mov qword [overflow_arg_area], rax
|
│           0x00401912      488d8550ff..   lea rax, [var_b0h]					; first param seems to be at 0xA8, but first actual (fixed) arg is at 0xF4
│           0x00401919      48898540ff..   mov qword [reg_save_area], rax
*/

/*	MACRO VA_ARG(list, typeof(arg))
│           0x00401923      8b8530ffffff   mov eax, dword [gp_offset]
│           0x00401929      83f82f         cmp eax, 0x2f
│       ┌─< 0x0040192c      7723           ja 0x401951
│       │   0x0040192e      488b8540ff..   mov rax, qword [reg_save_area]
│       │   0x00401935      8b9530ffffff   mov edx, dword [gp_offset]
│       │   0x0040193b      89d2           mov edx, edx
│       │   0x0040193d      4801d0         add rax, rdx
│       │   0x00401940      8b9530ffffff   mov edx, dword [gp_offset]
│       │   0x00401946      83c208         add edx, 8
│       │   0x00401949      899530ffffff   mov dword [gp_offset], edx
│      ┌──< 0x0040194f      eb12           jmp 0x401963
│      ││   ; CODE XREF from sym.f1 @ 0x40192c(x)
│      │└─> 0x00401951      488b8538ff..   mov rax, qword [overflow_arg_area]
│      │    0x00401958      488d5008       lea rdx, [rax + 8]
│      │    0x0040195c      48899538ff..   mov qword [overflow_arg_area], rdx
│      │    ; store result
│      └──> 0x00401963      488b00         mov rax, qword [rax]			; \__ store result
│           0x00401966      48898520ff..   mov qword [long_x], rax		; /

// overflow_arg_area probably represents the arguments pushed on the stack once you run out of registers
// similarly reg_save_area is where the registers are pushed on the stack. once you run out of space (fp_offset is never used but is probably the size of this segment) you go to overflow_arg_area
// gp_offset is the current offset within the reg_save_area. once that is above or equal to 0x30 === fp_offset, it stops being incremented and we go to overflow_arg_area
// **all** params are WORDSIZE === 8 bytes long, regardless of type size
// the call seems to handle a case where eax == 1 and smm registers are used, but it never seems to be the case

if gp_offset > 0x2F:
	rax = overflow_arg_area
	overflow_arg_area += 8
else
	rax = reg_save_area + gp_offset
	gp_offset += 8
res = *rax


// large structs (like here with sizeof(struct s1) == 24UL) seem to be pushed in any case
│      ┌──> 0x00401906      488b8528ff..   mov rax, qword [overflow_arg_area]
│      ╎│   0x0040190d      488d5018       lea rdx, [rax + 0x18]
│      ╎│   0x00401911      48899528ff..   mov qword [overflow_arg_area], rdx
│      ╎│   0x00401918      488b08         mov rcx, qword [rax]
│      ╎│   0x0040191b      488b5808       mov rbx, qword [rax + 8]
│      ╎│   0x0040191f      48898d00ff..   mov qword [var_100h], rcx
│      ╎│   0x00401926      48899d08ff..   mov qword [var_f8h], rbx
│      ╎│   0x0040192d      488b4010       mov rax, qword [rax + 0x10]
│      ╎│   0x00401931      48898510ff..   mov qword [var_f0h], rax
*/

/*
va_list structure starts with gp_offset
*/
// typedef struct arg_list {
// 	int gp_offset;
// 	int fp_offset;
// 	u64 reg_save_area;
// 	u64 overflow_arg_area;
// } va_list;
/* from https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf#subsection.3.5.7 */
// typedef struct {
// 	unsigned int gp_offset;
// 	unsigned int fp_offset;
// 	void *overflow_arg_area;
// 	void *reg_save_area;
// } c_va_list[1];

/*
 * 	The va_start macro initializes the structure as follows:
 * 	reg_save_area:
 * 		The element points to the start of the register save area.
 * 	overflow_arg_area:
 * 		This pointer is used to fetch arguments passed on the stack.
 * 		It is initialized with the address of the first argument passed on the stack, if
 * 		any, and then always updated to point to the start of the next argument on
 * 		the stack.
 * 	gp_offset:
 * 		The element holds the offset in bytes from reg_save_area to the
 * 		place where the next available general purpose argument register is saved.
 * 		In case all argument registers have been exhausted, it is set to the value 48
 * 		(6 ∗ 8).
 * 	fp_offset:
 * 		The element holds the offset in bytes from reg_save_area to the
 * 		place where the next available floating point argument register is saved. In
 * 		case all argument registers have been exhausted, it is set to the value 304
 * 		(6 ∗ 8 + 16 ∗ 16).
 */
// #define cst_va_start(ap, arg1) { ap->gp_offset = 8; ap->fp_offset = 0x30; ap->reg_save_area = &arg1; }

/*
 * given i don't have the power of the compiler, va_arg will probably have to be implemented with a few different macros (one for integers, one for floats, one for structs)
 */

// #define va_arg_int(ap);
// #define va_arg_float(ap);
// #define va_arg_struct(ap);

/*
 * my dude was ignored by the standard
 */
// #define va_end(ap)


// #define c_va_start(ap) do {  	asm(																		\
// 	"mov %0,     0x08"		/* gp_offset */															"\n\t"	\
// 	"mov %1,     0x30"		/* fp_offset */															"\n\t"	\
// 	"lea %2, 224[rsp]"		/* overflow_arg_area */													"\n\t"	\
// 	"lea %3,  32[rsp]"		/* reg_save_area */ 	 														\
// 	:																										\
// 		"=m"(ap->gp_offset), "=m"(ap->fp_offset), "=r"(ap->overflow_arg_area), "=r"(ap->reg_save_area) 		\
// ); } while(0)

/*
	if gp_offset <= 0x2F:
		rax = reg_save_area + gp_offset
		gp_offset += 8
	else
		rax = overflow_arg_area
		overflow_arg_area += 8
	res = *rax
*/
// #define c_va_arg_int(ap, out) do { asm(																		\
// 	"mov eax, dword %0"																				"\n\t"	\
// 	"cmp eax, 0x2f"																					"\n\t"	\
// 	"ja .+(0x401951-0x0040192c)"	/* ja else */													"\n\t"	\
// 	/* then */																								\
// 	"mov rax, qword %3"																				"\n\t"	\
// 	"mov edx, dword %0"																				"\n\t"	\
// 	"mov edx, edx"																					"\n\t"	\
// 	"add rax, rdx"																					"\n\t"	\
// 	"mov edx, %0"																					"\n\t"	\
// 	"add edx, 8"																					"\n\t"	\
// 	"mov dword %0, edx"																				"\n\t"	\
// 	"jmp .+(0x401963-0x0040194f)"	/* jmp store_result */											"\n\t"	\
// 	/* else */																								\
// 	"mov rax, %2"																					"\n\t"	\
// 	"lea rdx, [rax + 8]"																			"\n\t"	\
// 	"mov %2, rdx"																					"\n\t"	\
// 	/* store_result */																						\
// 	"mov rax, qword ptr [rax]"																		"\n\t"	\
// 	"mov %4, rax"																					"\n\t"	\
// 		: "=m"(ap->gp_offset), "=m"(ap->fp_offset), "=m"(ap->overflow_arg_area), "=m"(ap->reg_save_area),	\
// 		  "=m"(out)																							\
// ); } while(0)
