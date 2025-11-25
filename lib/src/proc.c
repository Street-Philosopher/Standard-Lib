
#include "../proc.h"

#include "../mem.h"
#include "../calls.h"
#include "../common.h"

int getpid() {
	SYSCALL_ASM_CALL(SYSCALL_GETPID)
}

int fork() {
	SYSCALL_ASM_CALL(SYSCALL_FORK)
}

int vfork() {
	SYSCALL_ASM_CALL(SYSCALL_VFORK)
}

int execve(char* file, char* argv[], char* envp[]) {
	SYSCALL_ASM_CALL(SYSCALL_EXECVE)
}

int wait4(int upid, int* stat_addr, int options, void* rusage) {
	SYSCALL_ASM_CALL(SYSCALL_WAIT4)
}

int kill(int pid, int sig) {
	SYSCALL_ASM_CALL(SUSCALL_KILL)
}

/*
Dump of assembler code for function sigaction:
   0x0000000000404550 <+0>:     endbr64
   0x0000000000404554 <+4>:     lea    eax,[rdi-0x1]
   0x0000000000404557 <+7>:     cmp    eax,0x3f
   0x000000000040455a <+10>:    ja     0x404570 <sigaction+32>
   0x000000000040455c <+12>:    lea    eax,[rdi-0x20]
   0x000000000040455f <+15>:    cmp    eax,0x1
   0x0000000000404562 <+18>:    jbe    0x404570 <sigaction+32>
   0x0000000000404564 <+20>:    jmp    0x4045b0 <__libc_sigaction>
   0x0000000000404569 <+25>:    nop    DWORD PTR [rax+0x0]
   0x0000000000404570 <+32>:    mov    rax,0xffffffffffffffc0			// return ENONET (??????)
   0x0000000000404577 <+39>:    mov    DWORD PTR fs:[rax],0x16
   0x000000000040457e <+46>:    mov    eax,0xffffffff
   0x0000000000404583 <+51>:    ret

Dump of assembler code for function __libc_sigaction:
   0x00000000004045b0 <+0>:     endbr64
   0x00000000004045b4 <+4>:     push   rbp
   0x00000000004045b5 <+5>:     mov    r8,rdx
   0x00000000004045b8 <+8>:     mov    rbp,rsp
   0x00000000004045bb <+11>:    sub    rsp,0x140
   0x00000000004045c2 <+18>:    mov    rax,QWORD PTR fs:0x28		// stack guard
   0x00000000004045cb <+27>:    mov    QWORD PTR [rbp-0x8],rax
   0x00000000004045cf <+31>:    xor    eax,eax
   0x00000000004045d1 <+33>:    test   rsi,rsi
   0x00000000004045d4 <+36>:    je     0x404730 <__libc_sigaction+384>
   0x00000000004045da <+42>:    mov    rax,QWORD PTR [rsi]
   0x00000000004045dd <+45>:    movdqu xmm0,XMMWORD PTR [rsi+0x8]
   0x00000000004045e2 <+50>:    lea    rdx,[rbp-0xa0]
   0x00000000004045e9 <+57>:    movdqu xmm1,XMMWORD PTR [rsi+0x18]
   0x00000000004045ee <+62>:    movdqu xmm2,XMMWORD PTR [rsi+0x28]
   0x00000000004045f3 <+67>:    mov    QWORD PTR [rbp-0x140],rax
   0x00000000004045fa <+74>:    movsxd rax,DWORD PTR [rsi+0x88]
   0x0000000000404601 <+81>:    movdqu xmm3,XMMWORD PTR [rsi+0x38]
   0x0000000000404606 <+86>:    movdqu xmm4,XMMWORD PTR [rsi+0x48]
   0x000000000040460b <+91>:    movups XMMWORD PTR [rbp-0x128],xmm0
   0x0000000000404612 <+98>:    or     rax,0x4000000
   0x0000000000404618 <+104>:   movdqu xmm5,XMMWORD PTR [rsi+0x58]
   0x000000000040461d <+109>:   movdqu xmm6,XMMWORD PTR [rsi+0x68]
   0x0000000000404622 <+114>:   movups XMMWORD PTR [rbp-0x118],xmm1
   0x0000000000404629 <+121>:   mov    QWORD PTR [rbp-0x138],rax
   0x0000000000404630 <+128>:   lea    rax,[rip+0xffffffffffffff69]        # 0x4045a0 <__restore_rt>
   0x0000000000404637 <+135>:   movdqu xmm7,XMMWORD PTR [rsi+0x78]
   0x000000000040463c <+140>:   lea    rsi,[rbp-0x140]
   0x0000000000404643 <+147>:   mov    QWORD PTR [rbp-0x130],rax
   0x000000000040464a <+154>:   xor    eax,eax
   0x000000000040464c <+156>:   test   r8,r8
   0x000000000040464f <+159>:   cmove  rdx,rax
   0x0000000000404653 <+163>:   movups XMMWORD PTR [rbp-0x108],xmm2
   0x000000000040465a <+170>:   movups XMMWORD PTR [rbp-0xf8],xmm3
   0x0000000000404661 <+177>:   movups XMMWORD PTR [rbp-0xe8],xmm4
   0x0000000000404668 <+184>:   movups XMMWORD PTR [rbp-0xd8],xmm5
   0x000000000040466f <+191>:   movups XMMWORD PTR [rbp-0xc8],xmm6
   0x0000000000404676 <+198>:   movups XMMWORD PTR [rbp-0xb8],xmm7
   0x000000000040467d <+205>:   mov    r10d,0x8
   0x0000000000404683 <+211>:   mov    eax,0xd
   0x0000000000404688 <+216>:   syscall
   0x000000000040468a <+218>:   cmp    rax,0xfffffffffffff000
   0x0000000000404690 <+224>:   ja     0x404750 <__libc_sigaction+416>
   0x0000000000404696 <+230>:   mov    edx,eax
   0x0000000000404698 <+232>:   test   r8,r8
   0x000000000040469b <+235>:   je     0x40471a <__libc_sigaction+362>
   0x000000000040469d <+237>:   test   eax,eax
   0x000000000040469f <+239>:   js     0x40471a <__libc_sigaction+362>
   0x00000000004046a1 <+241>:   mov    rax,QWORD PTR [rbp-0xa0]
   0x00000000004046a8 <+248>:   movdqu xmm0,XMMWORD PTR [rbp-0x88]
   0x00000000004046b0 <+256>:   movdqu xmm1,XMMWORD PTR [rbp-0x78]
   0x00000000004046b5 <+261>:   movdqu xmm2,XMMWORD PTR [rbp-0x68]
   0x00000000004046ba <+266>:   movdqu xmm3,XMMWORD PTR [rbp-0x58]
   0x00000000004046bf <+271>:   movdqu xmm4,XMMWORD PTR [rbp-0x48]
   0x00000000004046c4 <+276>:   mov    QWORD PTR [r8],rax
   0x00000000004046c7 <+279>:   movdqu xmm5,XMMWORD PTR [rbp-0x38]
   0x00000000004046cc <+284>:   movdqu xmm6,XMMWORD PTR [rbp-0x28]
   0x00000000004046d1 <+289>:   movups XMMWORD PTR [r8+0x8],xmm0
   0x00000000004046d6 <+294>:   mov    rax,QWORD PTR [rbp-0x98]
   0x00000000004046dd <+301>:   movdqu xmm7,XMMWORD PTR [rbp-0x18]
   0x00000000004046e2 <+306>:   movups XMMWORD PTR [r8+0x18],xmm1
   0x00000000004046e7 <+311>:   movups XMMWORD PTR [r8+0x28],xmm2
   0x00000000004046ec <+316>:   mov    DWORD PTR [r8+0x88],eax
   0x00000000004046f3 <+323>:   mov    rax,QWORD PTR [rbp-0x90]
   0x00000000004046fa <+330>:   movups XMMWORD PTR [r8+0x38],xmm3
   0x00000000004046ff <+335>:   mov    QWORD PTR [r8+0x90],rax
   0x0000000000404706 <+342>:   movups XMMWORD PTR [r8+0x48],xmm4
   0x000000000040470b <+347>:   movups XMMWORD PTR [r8+0x58],xmm5
   0x0000000000404710 <+352>:   movups XMMWORD PTR [r8+0x68],xmm6
   0x0000000000404715 <+357>:   movups XMMWORD PTR [r8+0x78],xmm7
   0x000000000040471a <+362>:   mov    rax,QWORD PTR [rbp-0x8]
   0x000000000040471e <+366>:   sub    rax,QWORD PTR fs:0x28
   0x0000000000404727 <+375>:   jne    0x404763 <__libc_sigaction+435>
   0x0000000000404729 <+377>:   leave
   0x000000000040472a <+378>:   mov    eax,edx
   0x000000000040472c <+380>:   ret
   0x000000000040472d <+381>:   nop    DWORD PTR [rax]
   0x0000000000404730 <+384>:   lea    rdx,[rbp-0xa0]
   0x0000000000404737 <+391>:   test   r8,r8
   0x000000000040473a <+394>:   jne    0x40467d <__libc_sigaction+205>
   0x0000000000404740 <+400>:   xor    edx,edx
   0x0000000000404742 <+402>:   xor    esi,esi
   0x0000000000404744 <+404>:   jmp    0x40467d <__libc_sigaction+205>
   0x0000000000404749 <+409>:   nop    DWORD PTR [rax+0x0]
   0x0000000000404750 <+416>:   mov    rdx,0xffffffffffffffc0
   0x0000000000404757 <+423>:   neg    eax
   0x0000000000404759 <+425>:   mov    DWORD PTR fs:[rdx],eax
   0x000000000040475c <+428>:   mov    edx,0xffffffff
   0x0000000000404761 <+433>:   jmp    0x40471a <__libc_sigaction+362>
   0x0000000000404763 <+435>:   call   0x419800 <__stack_chk_fail_local>


Setup I do not comprehend:
    mov rax, qword [rsi]              ; __sigaction_handler (value)
    movdqu xmm0, xmmword [rsi + 8]    ; sa_mask
    lea rdx, [var_a0h]                ; syscall.arg[rdx] = &var_A0h (address of outstruct on the stack)
    movdqu xmm1, xmmword [rsi + 0x18]
    movdqu xmm2, xmmword [rsi + 0x28]
    mov qword [var_140h], rax         ; <= __sigaction_handler
    movsxd rax, dword [rsi + 0x88] 	  ; sa_flags
    movdqu xmm3, xmmword [rsi + 0x38] ; 
    movdqu xmm4, xmmword [rsi + 0x48] ; 
    movups xmmword [var_128h], xmm0   ; <= sa_mask[0], sa_mask[1]
    or rax, 0x4000000
    movdqu xmm5, xmmword [rsi + 0x58] ; 
    movdqu xmm6, xmmword [rsi + 0x68] ; 
    movups xmmword [var_118h], xmm1   ; <= sa_mask[2], sa_mask[3]
    mov qword [var_138h], rax         ; <= modified sa_flags
    lea rax, [sym.__restore_rt]
    movdqu xmm7, xmmword [rsi + 0x78] ; 
    lea rsi, [var_140h]               ; __sigaction_handler
    mov qword [var_130h], rax         ; <= sym.__restore_rt
    xor eax, eax
    test r8, r8                       ; r8 was set to oldact before
    cmove rdx, rax                    ; if oldact == 0: syscall.arg[rdx] = 0
    movups xmmword [var_108h], xmm2
    movups xmmword [var_f8h], xmm3
    movups xmmword [var_e8h], xmm4
    movups xmmword [var_d8h], xmm5
    movups xmmword [var_c8h], xmm6
    movups xmmword [var_b8h], xmm7
Syscall:
    mov r10d, 8
    mov eax, 0xd
    syscall                           ; sys_rt_sigaction(signum, &var_140h, oldact != 0 ? var_A0h : NULL, 8)

┌ 16: sym.__restore_rt ();
│           0x0046a170      48c7c00f00..   mov rax, 0xf                ; sys_rt_sigreturn = 15
│           0x0046a177      0f05           syscall
└           0x0046a179      0f1f800000..   nop dword [rax]

Comprehension:
	movdqu = 128bit move, unaligned
	movups = same but for floats (?)
	movsxd = move with 32bit => 64bit sign extension

	mov rax, __sigaction_handler;

	example run of the program in GDB gave:
	&act:
	0x7fffffffdc00
	param for syscall:
	0x7fffffffdab0 (stack)

	at end of prep:
	-140h => __sigaction handler
	-138h => sa_flags | (1 << 26)
	-130h => sym.__restore_rt
	-128h => sa_mask

	why tf is it creating a new structure


size 0x98
s_sigact {
	0x00: __sigaction_handler (void(*)(int), size 8B)
	0x08: sa_mask (int[16], size 128B)
	0x88: sa_flags (int, size 8B)
	0x90: sa_restorer (void(*)(void), size 8B)
}
*/
typedef struct wtf_linux {
	sighandler_t handler;
	u64 modified_flags;
	void (*restorer)(void);
	__sigset_t sa_mask;
} actual_sigaction_t;

/*
wtf gcc?

lib/src/proc.c:220:18: error: expected ‘)’ before ‘::’ token
  220 |         "syscall"
      |                  ^
      |                  )
  221 |         :: "i"(SYSCALL_SIGRET)
      |         ~~        
lib/src/proc.c:216:5: note: to match this ‘(’
  216 | asm (
      |     ^
*/
asm (
".globl	restorer"																																																		"\n\t"
".type	restorer, @function"																																															"\n"
"restorer:"																																																				"\n\t"
	"mov rax,15"		/* SYSCALL_SIGRET */																																											"\n\t"
	"syscall"																																																			// "\n\t"
);
void restorer();

int rt_sigaction(int sig, const sigaction_t* act, sigaction_t* old_act) {

	// i was betrayed by the penguin

	actual_sigaction_t bro;
	bro.handler = act->sa_handler;
	bro.restorer = &restorer;

	asm("\n");

	bro.modified_flags = ((signed long)act->sa_flags) | (1 << 26);

	asm("\n");
	
	// copy the whole table thing
	bro.sa_mask = act->sa_mask;

	asm volatile("\nlea rax, %0\n\tmov rsi, rax" :: "m"(bro) );

	// syscall define for rt_sigaction literally starts with
	/*
		if (sigsetsize != sizeof(sigset_t))
			return -EINVAL;
	*/
	// wtf??
	asm volatile("\nmov r10, 0x08");
	// probably planning to do something more idk

	SYSCALL_ASM_CALL(SYSCALL_RT_SIGACT)
}
int sigaction(int sig, sighandler_t handler) {
	sigaction_t act;
	act.sa_handler = handler;
	return rt_sigaction(sig, &act, NULL);
}

int raise(int sig) {
	kill(getpid(), sig);
}
