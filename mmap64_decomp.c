#include <stdint.h>

#define PAGE_MASK 0x0000000000000FFF

#define EINVAL		22

extern int errno;

int64_t mmap (int64_t addr, int64_t len, int64_t prot, int64_t flags, int64_t fd, int64_t offset) {
    // rdi = addr;
    // rdx = prot;
    // rcx = flags;
    // r9  = offset;

    if ((offset & PAGE_MASK) != 0) {
        goto offset_error;
    }

    r12d = ecx;
    rbx = rdi;
    while (eax == 0) {
label_0:
        r10d = r12d;
        rdi = rbx;
		
        rax = mmap_syscall(addr, rsi, rdx, r10, r8, r9);

        if (rax > 0xfffffffffffff000) {		// -0x1000 => if -4096 < rax < 0
            goto label_3;
        }
label_1:
        r12 = rbx;
        return eax;
offset_error:
        errno = EINVAL;
        return -1;

        eax = edx;
        ecx &= 0x800;
        eax &= 4;
        eax |= ecx;
    }
    if ((*(0x004b0b15) & 4) == 0) {
        goto label_0;
    }
    r10d = r12d;
    edi = 0;
    eax = 9;
    r10d |= 0x40;
    rax = syscall_80h (rdi, rsi, rdx, r10, r8, r9);
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