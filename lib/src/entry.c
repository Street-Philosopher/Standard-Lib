/*
Dump of assembler code for function _start:
   0x0000000000001060 <+0>:     endbr64
   0x0000000000001064 <+4>:     xor    ebp,ebp
   0x0000000000001066 <+6>:     mov    r9,rdx
   0x0000000000001069 <+9>:     pop    rsi
   0x000000000000106a <+10>:    mov    rdx,rsp
   0x000000000000106d <+13>:    and    rsp,0xfffffffffffffff0
   0x0000000000001071 <+17>:    push   rax
   0x0000000000001072 <+18>:    push   rsp
   0x0000000000001073 <+19>:    xor    r8d,r8d
   0x0000000000001076 <+22>:    xor    ecx,ecx
   0x0000000000001078 <+24>:    lea    rdi,[rip+0xca]        # 0x1149 <main>
   0x000000000000107f <+31>:    call   QWORD PTR [rip+0x2f53]        # 0x3fd8
   0x0000000000001085 <+37>:    hlt
   */
// void _start() __attribute__((force_align_arg_pointer)) {}

//TODO: this feels stupid
asm (
	".text						\n\t"
	".globl	_start				\n\t"
	"_start:					\n\t\t"
	
	// setup
	
	// calling main function
	"call main					\n\t\t"

	// end of call
	"mov rdi,rax				\n\t\t"
	"call exit"
);
