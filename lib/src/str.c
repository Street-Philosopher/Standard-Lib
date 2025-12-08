
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
	char* retval = malloc(4);

	retval[0] = '0';
	retval[1] = 'x';
	retval[2] = '0' + ((num & 0xF0) >> 4);
	retval[3] = '0' + ((num & 0x0F) >> 0);

	return retval;
}
char* hexstr16(u16 num);
char* hexstr32(u32 num);
char* hexstr64(u64 num);


/*
 * the new string is created with `malloc`. remember to `free` it when you're done
 */
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
