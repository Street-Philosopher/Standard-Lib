	.file	"io.c"
	.intel_syntax noprefix
	.text
	.globl	strln
	.type	strln, @function
strln:
.LFB0:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	push	rbx
	.cfi_offset 3, -24
	mov	QWORD PTR -16[rbp], rdi
	mov	ebx, 0
	nop
.L2:
	mov	rax, rbx
	lea	rbx, 1[rax]
	mov	rdx, QWORD PTR -16[rbp]
	add	rax, rdx
	movzx	eax, BYTE PTR [rax]
	test	al, al
	jne	.L2
	mov	eax, ebx
	mov	rbx, QWORD PTR -8[rbp]
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	strln, .-strln
	.globl	print
	.type	print, @function
print:
.LFB1:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	QWORD PTR -8[rbp], rdi
	mov	QWORD PTR -16[rbp], rsi
#APP
# 18 "lib/src/io.c" 1
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 1
	mov rax, 1
	syscall
	
# 0 "" 2
#NO_APP
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	print, .-print
	.globl	print_nolen
	.type	print_nolen, @function
print_nolen:
.LFB2:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 8
	mov	QWORD PTR -8[rbp], rdi
	mov	rax, QWORD PTR -8[rbp]
	mov	rdi, rax
	call	strln
	movsx	rdx, eax
	mov	rax, QWORD PTR -8[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	print
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	print_nolen, .-print_nolen
	.globl	ioctl
	.type	ioctl, @function
ioctl:
.LFB3:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	DWORD PTR -4[rbp], edi
	mov	DWORD PTR -8[rbp], esi
	mov	QWORD PTR -16[rbp], rdx
	mov rax,QWORD PTR -16[rbp]		
	mov rdx,rax	
	mov rax,DWORD PTR -8[rbp]		
	mov rsi,rax	
	mov rax,DWORD PTR -4[rbp]		
	mov rdi,rax	
	mov rax,16
	syscall
	
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	ioctl, .-ioctl
	.globl	termios_set_bits
	.type	termios_set_bits, @function
termios_set_bits:
.LFB4:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 64
	mov	DWORD PTR -52[rbp], edi
	lea	rax, -48[rbp]
	mov	rdx, rax
	mov	esi, 21505
	mov	edi, 0
	call	ioctl
	mov	eax, DWORD PTR -36[rbp]
	or	eax, DWORD PTR -52[rbp]
	mov	DWORD PTR -36[rbp], eax
	lea	rax, -48[rbp]
	mov	rdx, rax
	mov	esi, 21506
	mov	edi, 0
	call	ioctl
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	termios_set_bits, .-termios_set_bits
	.globl	termios_reset_bits
	.type	termios_reset_bits, @function
termios_reset_bits:
.LFB5:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 64
	mov	DWORD PTR -52[rbp], edi
	lea	rax, -48[rbp]
	mov	rdx, rax
	mov	esi, 21505
	mov	edi, 0
	call	ioctl
	mov	eax, DWORD PTR -36[rbp]
	mov	edx, DWORD PTR -52[rbp]
	not	edx
	and	eax, edx
	mov	DWORD PTR -36[rbp], eax
	lea	rax, -48[rbp]
	mov	rdx, rax
	mov	esi, 21506
	mov	edi, 0
	call	ioctl
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	termios_reset_bits, .-termios_reset_bits
	.globl	getc
	.type	getc, @function
getc:
.LFB6:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	edi, 10
	call	termios_reset_bits
	lea	rax, -1[rbp]
	mov	esi, 1
	mov	rdi, rax
	call	input
	mov	edi, 10
	call	termios_set_bits
	movzx	eax, BYTE PTR -1[rbp]
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	getc, .-getc
	.globl	input
	.type	input, @function
input:
.LFB7:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	QWORD PTR -8[rbp], rdi
	mov	QWORD PTR -16[rbp], rsi
	mov rax, QWORD PTR -8[rbp]
	mov rsi, rax
	mov rdx, QWORD PTR -16[rbp]
	mov rdi, 0
	mov rax, 0
	syscall
	
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	input, .-input
	.globl	exit
	.type	exit, @function
exit:
.LFB8:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	DWORD PTR -4[rbp], edi
#APP
# 148 "lib/src/io.c" 1
	mov rax, 60
	syscall
	jmp exit
# 0 "" 2
#NO_APP
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	exit, .-exit
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
