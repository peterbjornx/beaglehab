/* 
 * This file is part of imud.
 *
 * imud is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * imud is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with imud.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	23/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need the libhab I2C API
 */
#include <hab/csi2c.h>

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

csi2c_bus_t *imud_sensor_bus;

void usage ( const char *reason )
{
	fprintf( stderr, 
		 "Syntax error: %s\nUsage: imud\n",
		 reason );
	exit ( EXIT_FAILURE );
}

int main( int argc, char **argv ) 
{

	/* Validate arguments */
	if ( argc != 1 )
		usage ( "argument count" );
	
	/* Enter main loop */	
	for ( ; ; ) {

		/* Wait for 100 milliseconds */
		usleep ( 100000 );		
	
	}


}
