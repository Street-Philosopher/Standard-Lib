.file	"entry.s"
.intel_syntax noprefix



.text
.globl	c_va_start
.type	c_va_start, @function
c_va_start:
	/* push	rbp */
	/* .cfi_def_cfa_offset 16 */
	/* .cfi_offset 6, -16 */
	/* mov	rbp, rsp */
	/* .cfi_def_cfa_register 6 */
	sub rsp,256

	mov	QWORD PTR -216[rbp], rdi
	mov	QWORD PTR -168[rbp], rsi		/* reg_save_area will point to here (actually, 8 bytes back) */
	mov	QWORD PTR -160[rbp], rdx
	mov	QWORD PTR -152[rbp], rcx
	mov	QWORD PTR -144[rbp], r8
	mov	QWORD PTR -136[rbp], r9
	test	al, al
	je	.L3
	movaps	XMMWORD PTR -128[rbp], xmm0
	movaps	XMMWORD PTR -112[rbp], xmm1
	movaps	XMMWORD PTR -96[rbp], xmm2
	movaps	XMMWORD PTR -80[rbp], xmm3
	movaps	XMMWORD PTR -64[rbp], xmm4
	movaps	XMMWORD PTR -48[rbp], xmm5
	movaps	XMMWORD PTR -32[rbp], xmm6
	movaps	XMMWORD PTR -16[rbp], xmm7
.L3:
	/* code for va_start */ 
	mov	DWORD PTR -200[rbp], 0x08		/* gp_offset */
	mov	DWORD PTR -196[rbp], 0x30		/* fp_offset */
	lea	rax, 16[rbp]
	mov	QWORD PTR -192[rbp], rax		/* overflow_arg_area */
	lea	rax, -176[rbp]
	mov	QWORD PTR -184[rbp], rax		/* reg_save_area */

	ret				/* this return goes to the area of va_start */ /* wtf does this mean i wrote this yesterday ffs */
.size c_va_start, .-c_va_start

/*
; .globl	c_va_arg_int
; .type	c_va_arg_int, @function
; c_va_arg_int:
; 	mov eax, dword [gp_offset]
; 	cmp eax, 0x2f
; 	ja .else

; 	mov rax, qword [reg_save_area]
; 	mov edx, dword [gp_offset]
; 	mov edx, edx
; 	add rax, rdx
; 	mov edx, dword [gp_offset]
; 	add edx, 8
; 	mov dword [gp_offset], edx
; 	jmp .endif

; 	.else:
; 	mov rax, qword [overflow_arg_area]
; 	lea rdx, [rax + 8]
; 	mov qword [overflow_arg_area], rdx

; 	.endif:
; 	mov rax, qword [rax]
; 	ret
; .size c_va_arg_int, .-c_va_arg_int
*/

.globl	c_va_arg_int
.type	c_va_arg_int, @function
c_va_arg_int:

	mov	rax, DWORD -196[rbp]

	ret
.size c_va_arg_int, .-c_va_arg_int



.globl	c_va_arg_end
.type	c_va_arg_end, @function
c_va_arg_end:

	mov	rax, DWORD -196[rbp]

	ret
.size c_va_arg_end, .-c_va_arg_end

