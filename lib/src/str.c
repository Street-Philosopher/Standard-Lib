
#include "../mem.h"
#include "../util.h"

#include "../debug.h"
 

int strcpy(char* from, char* to) {
	int i = 0;
	do {
		to[i] = from[i];
	} while (from[i++]);

	return i;
}
char* strcat(char* a, char* b) {
	int totlen = strln(a) + strln(b);
	char* retval = malloc(totlen);
	strcpy(b, retval - 1 + strcpy(a, retval));
	return retval;
}

int findoccurrences(char* str, char chr) {

	if (str[0] == 0) return 0;

	int retval = 0;
	int i = 0;
	bool waspressed = true;		// true in beginning to account for leading chars

	while (str[i]) {

		if (waspressed) {
			if (str[i] != chr) {
				waspressed = false;
			}
		} else {
			if (str[i] == chr) {
				waspressed = true;
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
char** splitstr(char* str, char sep) {

	/*
	 * array of pointers
	 * pt1, pt2, pt3, ..., 0x00 where sizeof(ptx) == 8
	 * sizeof(ptx) * (num_words + 1)
	 */
	char** retval;

	int num_of_tokens = findoccurrences(str, sep);
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
