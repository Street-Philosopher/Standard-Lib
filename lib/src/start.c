asm(
	".text			\n\t"
	".globl	_start	\n\t"
	"_start:		\n\t\t"
	"call main		\n\t\t"
	"mov rdi,0		\n\t\t"
	"call exit"
);
