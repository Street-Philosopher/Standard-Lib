#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/error.h"

const char* txt = "This is what\n\0";
const char txt2[] = "much /**/ much long strings\n\0";
const char yxt3[] = "this is the final and most longest of strings. almost as long as the line to see your mom on the highway\n\0";

int main() {

	char* string = malloc(16);
	printhex(string); newl();

	memcpy(txt, string, 16);
	// print(string);

	char* string2 = malloc(32);
	printhex(string2); newl();

	memcpy(txt2, string2, 32);
	// print(string2);

	char* string3 = malloc(0x80);
	printhex(string3); newl();

	memcpy(yxt3, string3, 0x80);
	// print(string3);

	// print(string3);
	// print(string2);
	// print(string);

	printhex(malloc(0x80));
}