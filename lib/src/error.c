#include "../error.h"

#include "../io.h"
#include "../common.h"
#include "../util.h"

void printerr(u64 err) {
	print(geterrname(err));
}
char* geterrname(u64 err) {
	// accept all format for error codes
	err = abs(err);

	switch (err) {
		#define X(a,b) case (b): return #a ;
		#include "error.lst"
		#undef X
		default:
			return "unknown";
	}
}