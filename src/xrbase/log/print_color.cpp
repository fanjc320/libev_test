/*
 *
 *      Author: venture
 */
#include "pch.h"

#ifdef _MSC_VER
#include <iostream>
#include <windows.h>
#else
#include <stdio.h>
#include "dirent.h"
#endif

#include "print_color.h"

#ifdef _MSC_VER
void set_console_color(WORD clr)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
}
#endif