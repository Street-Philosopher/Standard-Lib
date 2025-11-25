
#pragma once

#include "common.h"

//TODO: there is a range. i saw it somewhere. can't remember where
// this should be ERR_MAX: 

#define X(a,b) a = b,
// Y is for duplicates. we want them in here, but not in geterrname
#define Y X
typedef enum errornum_e {
	#include "src/error.lst"

	ERR_MAX = 4096
} errornum;
#undef X
#undef Y

#define iserr(x) ((u64)(x) > (u64)(-ERR_MAX))

void  printerr(u64 code);
char* geterrname(u64 code);
