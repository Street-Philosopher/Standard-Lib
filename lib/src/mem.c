#include "../mem.h"

void* malloc(u64 size);
void  free(void* ptr);
void* realloc(void* ptr, u64 size);

void memcpy(void* from, void* to, u64 size);
void memmov(void* from, void* to, u64 size);
void memset(void* ptr, u64 value, u64 size);
bool memcmp(void* ptr1, void* ptr2, u64 size);
