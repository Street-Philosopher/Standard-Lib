#include <stdint.h>

#define PAGE_MASK 0x0000000000000FFF

#define EINVAL		22

extern int errno;

int64_t mmap (int64_t addr, int64_t len, int64_t prot, int64_t flags, int64_t fd, int64_t offset) {
    // rdi = addr;
    // rdx = prot;
    // rcx = flags;
    // r9  = offset;

	int retval;

    if ((offset & PAGE_MASK) != 0) {
        goto offset_error;
    }

	// save this values for later restore, as RDI and R10D might get changed later
    // r12d = flags;
    // rbx = addr;
	if (addr == 0) {
		
	}
label_0:
        // r10d = flags;
        // rdi = addr;
		
		// RDI and R10D can change but are always set back to the saved values in rbx and r12, so this code always executes to the correct parameters
        retval = mmap_syscall(addr, len, prot, flags, fd, offset);

        if (retval > 0xfffffffffffff000) {		// -0x1000 -> if -4096 < rax < 0
            goto label_3;
        }

// could be return correctly
label_1:
        return retval;
		
offset_error:
        errno = EINVAL;
        return -1;

label_4:
        eax = edx;
        ecx &= 0x800;
        eax &= 4;
        eax |= ecx;
		goto label_0

    if ((*(0x004b0b15) & 4) == 0) {
        goto label_0;
    }
    r10d = r12d;
    edi = 0;
    eax = 9;
    r10d |= 0x40;
    rax = syscall_80h (addr, rsi, rdx, r10, fd, offset);
    if (rax <= 0xfffffffffffff000) {
        goto label_1;
    }
    rcx = 0xffffffffffffffc0;
    eax = -eax;
    *(fs:rcx) = eax;
    goto label_0;
label_3:
    rdx = 0xffffffffffffffc0;
    eax = -eax;
    *(fs:rdx) = eax;
    rax = 0xffffffffffffffff;
    goto label_1;
}