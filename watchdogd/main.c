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

void parse_config_line ( const char *line )
{
	char	*args[ 32 ];
	char	*line_copy = NULL;
	char	*token = NULL;
	char	*name = NULL;
	char	*path = NULL;
	char	*wdt = NULL;
	int	 wd_timeout;
	int	 ctr;

	/* Check for NULL pointers */
	csassert( line != NULL );

	/* Check if the line is a comment */
	if ( line[0] == '#' ) {

		/* The line is a comment, return */ 
		return;

	}

	/* Copy line into a mutable buffer */
	line_copy = malloc ( strlen ( line ) + 1 );

	/* Handle out of memory error */
	cserror( line_copy != NULL, 
		LOG_ERROR, 
		"Could not allocate line buffer");

	/* Grab the part of the line before the first equals sign */
	name = strtok ( line_copy, " =\t" );	 

	/* Check if the string was not empty */
	cserror( name != NULL, 
		LOG_ERROR, 
		"Invalid configuration line [%s]", 
		line_copy );

	/* Scan the remaining tokens */
	ctr = 0;
	while ( ( token = strtok ( NULL, " \t" ) ) != NULL ) {
		
		/* First token is the watchdog timeout ( in seconds ) */
		if ( ctr == 0 ) 
			wdt = token;
		/* Second token is the path to the binary */
		else if ( ctr == 1 )
			path = token;
		/* Remaining tokens are arguments */
		else if ( ctr < 34 ){
			args[ ctr - 2 ] = token;
		} else {
			/* We only support 32 arguments */

			cs_log_fatal (	LOG_ERROR, 
					"Too many tokens on line [%s]",
					line_copy );
		}
		
		ctr++;

	}
	
	/* Verify tokens */
	cserror( wdt != NULL, 
		 LOG_ERROR,
		 "No watchdog timeout specified on line [%s]",
		 line_copy );

	cserror( path != NULL, 
		 LOG_ERROR,
		 "No binary path specified on line [%s]",
		 line_copy );

	/* Add argument list terminator */
	args[ ctr - 2 ] = NULL;

	/* Parse watchdog timeout */
	wd_timeout = atoi ( wdt );
	
	/* Verify watchdog timeout */
	cserror( wd_timeout > 0, 
		 LOG_ERROR,
		 "Invalid watchdog timeout (%i) specified on line [%s]",
		 wd_timeout,
		 line_copy );

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

int main( int argc, char **argv ) 
{
	
	for ( ; ; ) {

		wdipc_process ( );
		pm_process ( );
		
	
	}


}
