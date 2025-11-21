
#include "util.h"

// thanks python3.Colorama

#define color(code) "\033[" stringify(code) "m"


#define COLOR_RESET_ALL 		0

// Standard
#define FORE_BLACK				30
#define FORE_RED				31
#define FORE_GREEN				32
#define FORE_YELLOW				33
#define FORE_BLUE				34
#define FORE_MAGENTA			35
#define FORE_CYAN				36
#define FORE_WHITE				37
#define FORE_RESET				39

#define BACK_BLACK				40
#define BACK_RED				41
#define BACK_GREEN				42
#define BACK_YELLOW				43
#define BACK_BLUE				44
#define BACK_MAGENTA			45
#define BACK_CYAN				46
#define BACK_WHITE				47
#define BACK_RESET				49

// These are fairly well supported, but not part of the standard.
#define FORE_LIGHTBLACK_EX		90
#define FORE_LIGHTRED_EX		91
#define FORE_LIGHTGREEN_EX		92
#define FORE_LIGHTYELLOW_EX		93
#define FORE_LIGHTBLUE_EX		94
#define FORE_LIGHTMAGENTA_EX	95
#define FORE_LIGHTCYAN_EX		96
#define FORE_LIGHTWHITE_EX		97

#define BACK_LIGHTBLACK_EX		100
#define BACK_LIGHTRED_EX		101
#define BACK_LIGHTGREEN_EX		102
#define BACK_LIGHTYELLOW_EX		103
#define BACK_LIGHTBLUE_EX		104
#define BACK_LIGHTMAGENTA_EX	105
#define BACK_LIGHTCYAN_EX		106
#define BACK_LIGHTWHITE_EX		107

// Style
#define STYLE_BRIGHT			1
#define STYLE_DIM				2
#define STYLE_NORMAL			22