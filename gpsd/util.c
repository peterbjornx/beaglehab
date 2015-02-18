/* 
	This file is part of gpsd.

	gpsd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gpsd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gpsd.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "gpsd.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

char *log_colours[LOG_MAX + 1] = {
	KCYN,
	KMAG,
	KWHT,
	KYEL,
	KRED
};

char *log_levels[LOG_MAX + 1] = {
	"TRACE",
	"DEBUG",
	" INFO",
	" WARN",
	"ERROR"
};


int log_level = LOG_TRACE;

void p_log ( int level, const char *format, ...)
{	
	char timestamp[25];
	time_t timer;
	struct tm* tm_info;
	va_list args;
	if (level < log_level)
		return;
	va_start ( args, format );
	time(&timer);	
	tm_info = localtime(&timer);
	strftime(timestamp, 25, "%H:%M:%S", tm_info);
	fprintf  ( stderr, "[%s] %s%s%s: ", timestamp, log_colours[level], log_levels[level], KNRM);
	vfprintf ( stderr, format, args );
 	va_end ( args );
} 
