
int strcpy(char* from, char* to);
char* strcat(char* a, char* b);

int findoccurrences(char* str, char chr);
char** splitstr(char* str, char sep);

// uses malloc
char* hexstr(u64 num);
// uses malloc
char* hexstr8 (u8  num);
// uses malloc
char* hexstr16(u16 num);
// uses malloc
char* hexstr32(u32 num);
// uses malloc
char* hexstr64(u64 num);
