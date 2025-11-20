#include "../error.h"
#include "../common.h"
#include "../util.h"

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