
#pragma once

#include "common.h"

typedef u32 tcflag_t;
typedef u8  cc_t;
#define NCCS 0x13
struct termios_t {
    tcflag_t c_iflag;               /* input mode flags */
    tcflag_t c_oflag;               /* output mode flags */
    tcflag_t c_cflag;               /* control mode flags */
    tcflag_t c_lflag;               /* local mode flags */
    cc_t c_line;                    /* line discipline */
    cc_t c_cc[NCCS];                /* control characters */
};

void termios_set_bits(u32 mask);
void termios_reset_bits(u32 mask);
