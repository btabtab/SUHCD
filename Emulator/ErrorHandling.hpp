#pragma once

#include <iostream>
#include <string>

enum ErrorCodes
{
	NO_ERROR,
	INTERNAL_SOFTWARE_WARNING,
	INTERNAL_SOFTWARE_ERROR,
};

void throwError(std::string name, int error_code);

/*Breakpointing functions.
*/

/*
	Sets up the next "breakThePoint()" call
	to set off a breakpoint
*/
void triggerBreakPoint();
void breakThePoint();