#include "../error.h"

char* geterrname(errornum err) {
	switch (err) {
		#define X(a,b) case (b): return #a ;
		#include "error.lst"
		#undef X
		default:
			return "unknown";
	}
}