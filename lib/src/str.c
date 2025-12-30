
#include "../mem.h"
#include "../util.h"

#include "../debug.h"
 
// this whole file was written in a single session trying to explain to a friend low level code
// as such it's not really optimised
// and i don't really feel like doing it any time soon tbh


int strcpy(char* from, char* to) {
	int i = 0;
	do {
		to[i] = from[i];
	} while (from[i++]);

	return i;
}


char* hexstr(u64 num) {

}
char* hexstr8 (u8  num) {
	char* retval = malloc(5);

	retval[0] = '0';
	retval[1] = 'x';
	retval[2] = '0' + ((num & 0xF0) >> 4);
	retval[3] = '0' + ((num & 0x0F) >> 0);
	retval[4] = 0;

	return retval;
}
char* hexstr16(u16 num);
char* hexstr32(u32 num);
char* hexstr64(u64 num) {
	
	char* outp;
	int pos_in_str = 2;
	u64 bitmask = 0xF000000000000000;
	
	// allocate for max possible size
	outp = malloc(sizeof("0x0000000000000000"));

	outp[0] = '0';
	outp[1] = 'x';
	outp[sizeof("0x0000000000000000")-1] = 0;

	while (pos_in_str < sizeof("0x0000000000000000")-1) {
		outp[pos_in_str] = (/* current digit value */(num & bitmask) >> /* current digit number */(60 + 4 * (2 - pos_in_str)));		// we initially shift 60 bits right, then 56 and so on. 4 is digit size, 60 is initial shift size, 2 is leading string length
		if (outp[pos_in_str] <= 9) {
			outp[pos_in_str] += '0';
		} else {
			outp[pos_in_str] += 'A' - 0xA;
		}

		pos_in_str++;
		bitmask >>= 4;
	}

	return outp;
}

char* itos(u64 value) {

	// num of digits is 20, + string terminator and sign if applicable
	char* outstr = malloc(20 + 2);

	const u64 sign_bitmsk = ((u64)1 << 63);

	bool print_zero_flag = false;
	u64  pow = 1E19, res;

	int pos_in_str = 0;

	debug_msg_addr(pow)

	// // if the sign bit is set print the sign and forget about it
	// if (value & sign_bitmsk) { outstr[0] = '-'; pos_in_str++; }

	do {
		res = value / pow;
		debug_msg_int(res);

		if (res || print_zero_flag) {
			print_zero_flag = true;
			value -= res * pow;
			outstr[pos_in_str++] = '0' + abs(res);
		}

		pow /= 10;
	} while (pow > 0);

	if (!print_zero_flag)				// if the print_zero_flag is still false we have not written anything yet. this is ugly. write zero
		outstr[0] = '0';

	return outstr;
}

char* strcat(char* a, char* b) {
	int totlen = strln(a) + strln(b);
	char* retval = malloc(totlen);
	strcpy(b, retval - 1 + strcpy(a, retval));
	return retval;
}

int findoccurrences(char* str, char chr) {
	int retval = 0;
	int i = 0;
	while (str[i]) {
		if (str[i] == chr) retval++;

		i++;
	}
	return retval;
}

int findspaces(char* str, char chr) {

	// prevents errors later on
	if (str[0] == 0) return 0;

	int retval = 0;
	int i = 0;
	bool waspressed = true;		// true in beginning to account for leading chars

	while (str[i]) {

		debug_msg_addr(str[i])

		if (waspressed) {
			if (str[i] != chr) {
				waspressed = false;
				debug_msg("set to false")
			}
		} else {
			if (str[i] == chr) {
				waspressed = true;
				debug_msg("increase retval")
				retval++;
			}
		}
		
		i++;
	}

	if ( /* i > 0 && */ str[i-1] == chr) {
		retval--;	// to account for trailing chars
	}

	// we're counting num of chars, so add 1
	return ++retval;
}

/*
 * both the return value and the individual strings are created using `malloc`. remember to `free` the pointers once you're done
 */
char** splitstr(char* str, char sep) {

	/*
	 * array of pointers
	 * pt1, pt2, pt3, ..., 0x00 where sizeof(ptx) == 8
	 * sizeof(ptx) * (num_words + 1)
	 */
	char** retval;

	int num_of_tokens = findspaces(str, sep);
	retval = malloc(WORDSIZE * (1 + num_of_tokens));

	if (num_of_tokens == 0) {
		// return empty list
	}

	// TODO: it's the same loop as above
	int prev;					// current token's start index
	int curtokenlen;
	int curtoken = 0;
	int i = 0;
	bool waspressed = true;		// true in beginning to account for leading chars
	while (str[i]) {

		if (waspressed) {
			if (str[i] != sep) {
				prev = i;
				waspressed = false;
			}
		} else {
			if (str[i] == sep) {
				waspressed = true;

				// tokenlength
				curtokenlen = (i - prev);
				retval[curtoken] = malloc(curtokenlen);
				memcpy(&str[prev], retval[curtoken], curtokenlen);

				curtoken++;
			}
		}
		
		i++;
	}

	if (str[i-1] != sep) {
		// TODO: same as above
		curtokenlen = (i - prev);
		retval[curtoken] = malloc(curtokenlen);
		memcpy(&str[prev], retval[curtoken], curtokenlen);
	}

	return retval;

}

