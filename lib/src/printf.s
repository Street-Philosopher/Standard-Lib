.equ ERR_MAX, 0xFFFFFFFFFFFFF000

	.intel_syntax noprefix
	.text

/* fuck it i'm writing printf in assembly */
/* this is so bad we scan the string like thrice */
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
	sub	rsp, 192 /* for reg_save_area */ + 8 /* for local variables */

	/* |------- LOCAL VARIABLES -------| */
	/* | &formatters_array = -192[rbp] | */

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
	create_str_array:
		sal rax,3			/* valid bc sizeof(char*) == 8 */
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
			cmp rax, 0x2F
			je .get_replacement_str
			/* increase total string length by just one as we copied one char without expanding */
			add rbx,1
			jmp .check_str
			.get_replacement_str:
				/* call printf_expand_d with (indexof(char), rbp, nextstr) */
				mov rdi, rbp
				mov rsi, rcx
				/* rdx is already pointing the string */
				/* IMPORTANT: rdx is pointing to the next char. handle it correctly in the function */
				call printf_expand_d
				// TODO: since we have to call strcpy later anyways maybe we can return the value we need to increment rdx instead? idk

				/* FIXME: how much should we increment rdx? should be based on info obtained from printf_expand_d */

				/* change total string length by the length of the expanded str */
				add rbx, rax
				/* next entry in the string array */
				add rcx, 8
			/* fall through */
		.check_str:
		mov al, BYTE PTR [rdx]
		movzx rax,al
		add rdx,1
		test rax, rax
		jne .find_and_replace_body

	/* create buffer for total string */
	/* null terminator not necessary as `write` requires buffer length */
	mov rdi,rbx
	call malloc
	cmp rax,ERR_MAX			/* \___ if iserr(retcode) => return retcode	*/
	jae return				/* / 										*/
	
	/* copy and print */
	final_copy:
		mov r10,rax				/* save address of str to print for later, as we're going to change it */

		mov r8, rax						/* this will be used by the loop to know where to write to */
		mov r9, QWORD PTR -184[rbp]		/* this is the format string used to write no */
		/* at this point only rcx is freee of all the gp registers i beliebe */
		/*
		for char in str:
			if char is not '%':		// idk maybe the condition is backwards (i'm stoned)
				newbuf = char
			else
				strcpy (rax, string_array[next])
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
				/* FIXME: when decoding a formatter, replace every characteer but % of the instructions to zero in this section we'll loop over format til we find no more zeros i swar i did not control myslef while writing that */
				/* since format is probably not writeable, maybe keep it in the thing array instead, like a struct. maybe this can solve the fixme/todo above */

			/* fall through */
		.check_str2:
		mov al, BYTE PTR [r9]			/* 	al = arg1[cur]		 */
		movzx rax,al					/*	necessary?			 */
		test rax, rax
		jne .final_copy_body
	

	mov rsi, r10		/* addr of string to print */
	mov rdi, 1			/* fd = stdout */
	mov rdx, rbx		/* mov rdx,total_length */
	call write
	/* rax now contains number of bytes written to stdout */
	
	/* always jump / fall through to this label if you've already set the return value in `rax` */
	return:
	push rax			/* `free` will alter rax */

	/* free bros */
	mov rdi, r10					/* addr of str to ptint */
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
	.back:
	leave
	ret
	/* error during free. yikes */
	.very_bad:
		mov rbx,rax
		mov rdi,catastrophe_str
		call print
		mov rdi, rbx
		call printint
		call newl
		jmp .back
.size	check_free_errors, .-check_free_errors



/* RODATA */
.section	.rodata

catastrophe_str:
	.string	"\033[31m!!! Error while freeing ptr inside printf !!!\033[0m\nError code: "
