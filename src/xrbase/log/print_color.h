/*
 *
 *      Author: venture
 */

#pragma once

#include <iostream>
#include "string/str_format.h"

#ifdef _MSC_VER
#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.
#define  CLR_YELLOW FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN
#define  CLR_RED FOREGROUND_INTENSITY | FOREGROUND_RED
#define  CLR_GREEN FOREGROUND_INTENSITY | FOREGROUND_GREEN
#define  CLR_BLUE FOREGROUND_INTENSITY | FOREGROUND_BLUE
#define  CLR_RESET FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

extern void set_console_color(WORD clr);
#else
#define CLR_RESET "\033[0m"
#define CLR_BLACK "\033[30m" /* Black */
#define CLR_RED "\033[31m" /* Red */
#define CLR_REDBACK "\033[41m" /* Red BACK*/
#define CLR_GREEN "\033[32m" /* Green */
#define CLR_YELLOW "\033[33m" /* Yellow */
#define CLR_BLUE "\033[34m" /* Blue */
#define CLR_MAGENTA "\033[35m" /* Magenta */
#define CLR_CYAN "\033[36m" /* Cyan */
#define CLR_WHITE "\033[37m" /* White */
#define CLR_BOLDBLACK "\033[1m\033[30m" /* Bold Black */
#define CLR_BOLDRED "\033[1m\033[31m" /* Bold Red */
#define CLR_BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define CLR_BOLDYELLOW "\033[1m\033[33m" /* Bold Yellow */
#define CLR_BOLDBLUE "\033[1m\033[34m" /* Bold Blue */
#define CLR_BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define CLR_BOLDCYAN "\033[1m\033[36m" /* Bold Cyan */
#define CLR_BOLDWHITE "\033[1m\033[37m" /* Bold White */
#endif //

template <typename T, typename... Args>
inline void print_color(T clr, const char* szFormat, Args&&... args)
{
#ifdef _MSC_VER
	set_console_color(clr);
	printf_s(szFormat, std::forward<Args>(args)...);
	set_console_color(CLR_RESET);
	fflush(stdout);
#else
	printf_s("%s", clr);
	printf_s(szFormat, std::forward<Args>(args)...);
	printf_s("%s", CLR_RESET);
	fflush(stdout);
#endif // _MSC_VER

}
