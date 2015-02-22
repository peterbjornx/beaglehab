/* 
 * This file is part of libhab.
 *
 * libhab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * libhab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with libhab.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	22/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need stdlib for process management functions
 */
#include <stdlib.h>

/*
 * We need stdio for the log I/O
 */
#include <stdio.h>

/*
 * We need stdarg for the variadic log function
 */
#include <stdarg.h>

/*
 * We need time for the log timestamps
 */
#include <time.h>

/*
 * ANSI Colour escape sequences
 */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


/*
 * Log level colour assignments
 */
char *log_colours[LOG_MAX + 1] = {
	KCYN,
	KMAG,
	KWHT,
	KYEL,
	KRED
};

/*
 * Log level names
 */
char *log_levels[LOG_MAX + 1] = {
	"TRACE",
	"DEBUG",
	" INFO",
	" WARN",
	"ERROR"
};

/**
 * The minimum log level displayed by the software
 */
int cs_log_level = LOG_TRACE;

/**
 * @brief Set the minimum log level
 * @param log_level The minimum log level displayed by the software
 */
void cs_set_log_level( int log_level )
{
	cs_log_level = log_level;
}

/**
 * @brief Output a message to the log
 * @param level The level of the message
 * @param format The format string to output ( see *printf* )
 */
void cs_log ( int level, const char *format, ...)
{	
	char timestamp[25];
	time_t timer;
	struct tm* tm_info;
	va_list args;

	/* Check whether we should display this message */
	if (level < log_level)
		return;

	/* Initialize varargs */
	va_start ( args, format );

	/* Grab the current time */
	time(&timer);	

	/* Convert that time to the local timezone */
	tm_info = localtime(&timer);

	/* Format the timestamp */
	strftime(timestamp, 25, "%H:%M:%S", tm_info);

	/* Print the log prefix [<time>] <colour><level><endcolour>: */
	fprintf  ( stderr, "[%s] %s%s%s: ", timestamp, log_colours[level], log_levels[level], KNRM);
	
	/* Print the message */
	vfprintf ( stderr, format, args );

	/* Release varargs */
 	va_end ( args );
} 

/**
 * @brief Output a message to the log and die
 * @param level The level of the message
 * @param format The format string to output ( see *printf* )
 */
void cs_log_fatal ( int level, const char *format, ...)
{	
	char timestamp[25];
	time_t timer;
	struct tm* tm_info;
	va_list args;

	/* Check whether we should display this message */
	if (level < log_level)
		exit(EXIT_FAILURE);

	/* Initialize varargs */
	va_start ( args, format );

	/* Grab the current time */
	time(&timer);	

	/* Convert that time to the local timezone */
	tm_info = localtime(&timer);

	/* Format the timestamp */
	strftime(timestamp, 25, "%H:%M:%S", tm_info);

	/* Print the log prefix [<time>] <colour><level><endcolour>: */
	fprintf  ( stderr, "[%s] %s%s%s: ", timestamp, log_colours[level], log_levels[level], KNRM);
	
	/* Print the message */
	vfprintf ( stderr, format, args );

	/* Release varargs */
 	va_end ( args );

	/* Terminate unsucessfully */
	exit(EXIT_FAILURE);
} 
