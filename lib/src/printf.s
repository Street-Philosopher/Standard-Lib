.equ ERR_MAX, 0xFFFFFFFFFFFFF000

	.intel_syntax noprefix
	.text

/* fuck it i'm writing printf in assembly */
/* this is so bad we scan the string like thrice */
/* i did it like this because i thought calling `write` multiple times would be bad, but oh boy is this worse */
/* i know this is catastrophic, but hey at least i'm calling `write` only once */

	.globl	printf
	.type	printf, @function
printf:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 192 /* for reg_save_area */ + 16 /* for local variables */

	/* |------- LOCAL VARIABLES -------| */
	/* | &formatters_array = -192[rbp] | */
	/* | &string_to_output = -200[rbp] | */

	/* format str */
	mov	QWORD PTR -184[rbp], rdi
	/* reg_save_area starts from here (technically from -174[rbp]) */
	mov	QWORD PTR -168[rbp], rsi
	mov	QWORD PTR -160[rbp], rdx
	mov	QWORD PTR -152[rbp], rcx
	mov	QWORD PTR -144[rbp], r8
	mov	QWORD PTR -136[rbp], r9
	test	al, al
	je	.nofloats
	movaps	XMMWORD PTR -128[rbp], xmm0
	movaps	XMMWORD PTR -112[rbp], xmm1
	movaps	XMMWORD PTR -96[rbp], xmm2
	movaps	XMMWORD PTR -80[rbp], xmm3
	movaps	XMMWORD PTR -64[rbp], xmm4
	movaps	XMMWORD PTR -48[rbp], xmm5
	movaps	XMMWORD PTR -32[rbp], xmm6
	movaps	XMMWORD PTR -16[rbp], xmm7
	.nofloats:

	/* start by checking how many string expansions there are */
	find_percent_signs:
		/* rdi already contains the string. just move '%' in rsi */
		mov rsi, '%'
		call findoccurrences

	/* create a string array to hold all the % expansions */
	/* it's not actually a string array, more like an array of structs like so: */
	/*
	 * struct formattedstr {
	 *     u64 formatterlength;		// length of the formatter used to generate the string itself, for example '3' for "%dxu" (which is hexadecimal unsigned integer)
	 *     char* string;			// string itself
	 * }
	 */
	create_str_array:
		/* valid bc sizeof(char*) == sizeof(u64) == 8 => sizeof(struct formattedstr) == 16. malloc (numofentries * sizeof(char*) * 2) */
		sal rax,4
		mov rdi,rax
		/* should printf ever fail? no. thus, should printf use malloc? also no. but it's my garbage program and i get to cause undefined behaviour if i want to (i spent like a week working on that malloc function and i want to use it ok) */
		call malloc		
		cmp rax,ERR_MAX			/* \___ if iserr(retcode) => return retcode	*/
		jae return				/* / 										*/
		mov QWORD PTR -192[rbp], rax	/* save for later */

	/* for each % in the string, expand and store it in the thing */
	expand_formatters:
		/*
		for char in str:
			if char is '%':
				call printf_expand_d with (indexof(char), va_next_arg)
				formatters_array[next] = retval
		*/
		/* rbx contains total formatted string length */
		xor rbx, rbx
		/* rcx contains pointer to array of strings,  */
		mov rcx, rax
		/* rdx contains pointer to format string */
		mov rax, QWORD PTR -184[rbp]
		mov rdx, rax
		jmp .check_str
		.find_and_replace_body:
			/* if char is '%': (else nop) */
			/* `mov al, BYTE PTR [rdx]` not necessary: we have already rax, and rdx is now actually poynting to the next char */
			cmp rax, '%'
			je .get_replacement_str
			/* increase total string length by just one as we copied one char without expanding */
			add rbx,1
			add rdx,1
			jmp .check_str
			.get_replacement_str:
				/* call printf_expand_d (from file printf.c) with (indexof(char), rbp, nextstr) */
				mov rdi, rbp
				mov rsi, rcx
				/* rdx is already pointing the string */
				/* IMPORTANT: rdx is pointing to the next char. handle it correctly in the function */
				push rbx
				push rcx
				push rdx
				call printf_expand_d		/* TODO: check if return error (actually no since we just UD our way out) */
				pop rdx
				pop rcx
				pop rbx

				/* first half of the struct contains length of formatter str read.
				 * second contains the pointer to the decoded str, which we don't care about rn */
				mov r8, QWORD PTR[rcx]

				/* change total string length by the length of the expanded str */
				add rbx, rax
				/* next entry in the thing array */
				add rcx, 16
				/* format string is incremented by the length of the section we used to tell the formatter function what to do (e.g.: "%dxu" => increment by 3, the string is an unsigned dword printed as a hex number) */
				add rdx, r8
			/* fall through */
		.check_str:
		mov al, BYTE PTR [rdx]
		movzx rax,al
		test rax, rax
		jne .find_and_replace_body

	/* create buffer for total string */
	/* null terminator not necessary as `write` requires buffer length */
	mov rdi,rbx
	call malloc
	cmp rax,ERR_MAX			/* \___ if iserr(retcode) => return retcode	*/
	jae return				/* / 										*/

	mov -200[rbp], rax				/* save address of str to print for later, as we're going to change it */
	
	/* copy and print */
	final_copy:
		mov r8, rax						/* this will be used by the loop to know where to write to */
		mov r9, QWORD PTR -184[rbp]		/* this is the format string used to write no */
		mov r10, QWORD PTR -192[rbp]	/* array of formatted substrings */
		/* at this point only rcx is freee of all the gp registers i beliebe */
		/*
		for char in str:
			if char is not '%':		// idk maybe the condition is backwards (i'm stoned)
				newbuf = char
			else
				strcpy (rax, formatters_array[next].string)
				rcx += (* num of bytes copied *)  // comments within comments. fuck jml
		*/
		jmp .check_str2
		.final_copy_body:
			cmp rax,'%'
			je .fcb_copy_formatted_str
			/* copy normal char */
			.fcb_else:
				mov BYTE PTR[r8], al		/* outstr[next] = formatstr[next] */

				/* point to next*/
				add r8,1
				add r9,1
				jmp .check_str2

			/* copy string expansion */
			.fcb_copy_formatted_str:
				/* strcpy.to = output buffer */
				mov rsi, r8
				/* get formatters_array[next].formatterlength, and increase format string position accordingly */
				mov rax, QWORD PTR[r10]
				add r9, rax
				/* strcpy.from = formatters_array[next].string */
				add r10, 8
				mov rdi, QWORD PTR [r10]
				push r9
				push r10
				push r8
				call strcpy
				pop r8
				sub rax, 1				/* to account for terminating null char */
				add r8, rax				/* position in string we are currently writing */
				push r8
				/* would have to do `mov rdi, rdi` but don't bc it's useless. we free the string we don't need anymore so we don't have to do it later */
				call free
				pop r8
				pop r10
				pop r9
				call check_free_errors	/* TODO: this is ugly here, can i move it up? */
				add r10, 8				/* go to next entry. not checking for segfaults, that's on the user to not be stupid */

			/* fall through */
		.check_str2:
		mov al, BYTE PTR [r9]			/* 	al = arg1[cur]		 */
		movzx rax,al					/*	necessary?			 */
		test rax, rax
		jne .final_copy_body
	

	mov rsi, -200[rbp]	/* addr of string to print */
	mov rdi, 1			/* fd = stdout */
	mov rdx, rbx		/* mov rdx,total_length */
	call write
	/* rax now contains number of bytes written to stdout */
	
	/* always jump / fall through to this label if you've already set the return value in `rax` */
	return:
	push rax			/* `free` will alter rax */

	/* free bros */
	mov rdi, -200[rbp]				/* addr of str to ptint */
	call free
	call check_free_errors
	mov rdi,QWORD PTR -192[rbp]		/* thing array */
	call free
	call check_free_errors

	/* hopefully free works lol */
	/* can't really check if it did. we can but can't do anything with the result, as we can't return an error even though we have printed chars on the string */
	/* and i mean we can't just loop until `free` works lol */
	/* maybe add a `warning code` global var just to keep count of how many memory leaks you have */

	pop rax
	leave
	.cfi_def_cfa 7, 8
	ret
.cfi_endproc
.size	printf, .-printf


	.type check_free_errors, @function
check_free_errors:
	cmp rax,ERR_MAX			/* if iserr(retcode) => handle	*/
	jae .very_bad
	ret
	/* error during free. yikes */
	.very_bad:
		push rbx
		mov rdi,catastrophe_str
		call print
		pop rbx
		call printint
		call newl
		ret
.size	check_free_errors, .-check_free_errors



/* RODATA */
.section	.rodata

catastrophe_str:
	.string	"\033[31m!!! Error while freeing ptr inside printf !!!\033[0m\nError code: "
