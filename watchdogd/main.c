/* 
 * This file is part of watchdogd.
 *
 * watchdogd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * watchdogd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with watchdogd.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	22/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need the process manager header
 */
#include "procmgr.h"

/*
 * We need the watchdog ipc header
 */
#include "wdipc.h"

/*
 * We need hab/cslog for logging
 */
#include <hab/cslog.h>

/*
 * We need stdlib for the exit function and for memory management
 */
#include <stdlib.h>

/*
 * We need errno to handle errors that occurred in system calls
 */
#include <errno.h>

/*
 * We need string to generate friendly names for errors
 */
#include <string.h>

/*
 * We need stdio for reading the configuration file
 */
#include <stdio.h>

/*
 * We need unistd for the microsecond sleep
 */
#include <unistd.h>

void parse_config_line ( const char *line )
{
	char	*args[ 33 ];
	char	*line_copy = NULL;
	char	*token = NULL;
	char	*name = NULL;
	char	*path = NULL;
	char	*wdt = NULL;
	int	 wd_timeout;
	int	 ctr;

	/* Check for NULL pointers */
	csassert( line != NULL );

	/* Check if the line is a comment or empty */
	if ( (line[0] == '#') || (line[0] == '\0') ) {

		/* The line is a comment, return */ 
		return;

	}

	/* Copy line into a mutable buffer */
	line_copy = malloc ( strlen ( line ) + 1 );

	/* Handle out of memory error */
	cserror( line_copy != NULL, 
		LOG_ERROR, 
		"Could not allocate line buffer");

	/* Copy the line */
	strcpy ( line_copy, line );

	/* Grab the part of the line before the first equals sign */
	name = strtok ( line_copy, " \t\n" );	 

	/* Check if the string was not empty */
	if ( name == NULL ) {
		
		/* The line was empty */
	
		/* Free variables */
		free ( line_copy );
		
		/* Return */
		return;
	}

	/* Scan the remaining tokens */
	ctr = 0;
	while ( ( token = strtok ( NULL, " \t\n" ) ) != NULL ) {
		
		/* First token is the watchdog timeout ( in seconds ) */
		if ( ctr == 0 ) 
			wdt = token;
		/* Second token is the path to the binary */
		else if ( ctr == 1 )
			path = token;
		/* Remaining tokens are arguments */
		else if ( ctr < 34 ){
			args[ ctr - 1 ] = token;
		} else {
			/* We only support 32 arguments */

			cs_log_fatal (	LOG_ERROR, 
					"Too many tokens on line [%s]",
					line );
		}
		
		ctr++;

	}
	
	/* Verify tokens */
	cserror( wdt != NULL, 
		 LOG_ERROR,
		 "No watchdog timeout specified on line [%s]",
		 line );

	cserror( path != NULL, 
		 LOG_ERROR,
		 "No binary path specified on line [%s]",
		 line );

	/* Add argv[0] */
	args[0] = path;

	/* Add argument list terminator */
	args[ ctr - 1 ] = NULL;

	/* Parse watchdog timeout */
	wd_timeout = atoi ( wdt );
	
	/* Verify watchdog timeout */
	cserror( wd_timeout > 0, 
		 LOG_ERROR,
		 "Invalid watchdog timeout (%i) specified on line [%s]",
		 wd_timeout,
		 line );

	/* Log this event */
	cs_log( LOG_INFO, 
		"Adding process %s with timeout %i", 
		name, 
		wd_timeout );	

	/* Add process to manager */
	pm_add_process ( name, wd_timeout, path, ( const char **) args );

	/* Free variables */
	free ( line_copy );

}

void parse_config_file ( const char *path )
{
	char line_buffer[161];
	FILE *config_file;

	/* Check for NULL pointers */
	csassert( path != NULL );

	/* Attempt to open the file */
	config_file = fopen ( path, "r" );

	/* Check for errors */
	cserror( config_file != NULL, 
		 LOG_ERROR,
		 "Could not open config file (%s): %i(%s)",
		 path,
		 errno,
		 strerror( errno ) );
	
	/* Read all lines from the file */
	while ( fgets ( line_buffer, 160, config_file ) != NULL ) {

		/* Parse the line */
		parse_config_line ( line_buffer );
		
	}

	/* Check for errors */
	cserror( ferror ( config_file ) == 0 , 
		 LOG_ERROR,
		 "Could not read from config file (%s): %i(%s)",
		 path,
		 errno,
		 strerror ( errno ) );

	
	/* Close the file */
	cserror( fclose ( config_file ) == 0,
		 LOG_WARN,
		 "Could not close config file (%s): %i(%s)",
		 path,
		 errno,
		 strerror ( errno ) );
	
}

void usage ( const char *reason )
{
	fprintf( stderr, 
		 "Syntax error: %s\nUsage: watchdogd <config file path>\n",
		 reason );
	exit ( EXIT_FAILURE );
}

int main( int argc, char **argv ) 
{

	/* Validate arguments */
	if ( argc != 2 )
		usage ( "argument count" );

	/* Initialize process manager */	
	pm_initialize ( );
	
	/* Load configuration */
	parse_config_file ( argv[1] );

	/* Initialize watchdog IPC */
	wdipc_initialize ( );
	
	/* Enter main loop */	
	for ( ; ; ) {

		/* Process watchdog timer reset requests */
		wdipc_process ( );

		/* Process process events */
		pm_process ( );

		/* Wait for 100 milliseconds */
		usleep ( 100000 );		
	
	}


}
