
#pragma once

#define stringify(obj) #obj

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]));
#define ASSERT(condition) BUILD_BUG_ON(!(condition))

#define SECTION(name) name:

int abs(int);
u64 strln(char* str);