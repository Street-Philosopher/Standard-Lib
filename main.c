#include "lib/io.h"
#include "lib/os.h"
#include "lib/mem.h"
#include "lib/error.h"

const char* txt = "This is what\n\0";
const char txt2[] = "much /**/ much long strings\n\0";
const char yxt3[] = "this is the final and most longest of strings. almost as long as the line to see your mom on the highway\n\0";

int main() {

	#define size (16)

	char* x = malloc(size);
	print("x\n");
	printhex(x); newl();
	for (int i = 0; i < size; i++) {
		x[i] = 'x';
	}

	char* y = malloc(size);
	print("y\n");
	printhex(y); newl();
	for (int i = 0; i < size; i++) {
		y[i] = 'y';
	}
	
	char* z = malloc(2*size);
	print("z\n");
	printhex(z); newl();
	for (int i = 0; i < 2*size; i++) {
		z[i] = 'z';
	}

	printhex(y);  newl();
	free(z);
	free(y);

	print("freed bros\n");newl();

	print("x2\n");
	char* x2 = malloc(size);
	print2(x2, size); newl();

	printhex(x2); newl();

	print("x3\n");
	char* x3 = malloc(2*size);
	print2(x3, 2*size); newl();
	printhex(x3); newl();

	print("x4\n");
	char* x4 = malloc(size);
	print2(x4, size);newl();
	printhex(x4); newl();

	print("-ok"); newl();

	return 0;

	#ifdef gianni
	char* string = malloc(16);
	printhex(string); newl();

	memcpy(txt, string, 16);
	print(string);

	char* string2 = malloc(32);
	printhex(string2); newl();

	memcpy(txt2, string2, 32);
	print(string2);

	char* string3 = malloc(0x80);
	printhex(string3); newl();

	memcpy(yxt3, string3, 0x80);
	print(string3);

	print(string3);
	print(string2);
	print(string);

	printhex(malloc(0x80));
	#endif
}