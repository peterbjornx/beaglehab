/* 
 * This file is part of barod.
 *
 * barod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * barod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with barod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	24/02/2015
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
 * We need hab/csdata for IPC data 
 */
#include <hab/csdata.h>

/*
 * We need hab/csipc for the libhab IPC system
 */
#include <hab/csipc.h>

/*
 * We need hab/cswdog for the watchdog reset function
 */
#include <hab/cswdog.h>

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

/*
 * We need time for the timestamps
 */
#include <time.h>

/*
 * We need the barometer functions
 */
#include "baro.h"

csi2c_bus_t	*barod_sensor_bus;
cs_srv_t	*barod_ipc_server;
csproto_baro_t  *barod_packet;

void usage ( const char *reason )
{
	fprintf( stderr, 
		 "Syntax error: %s\nUsage: barod\n",
		 reason );
	exit ( EXIT_FAILURE );
}

int main( int argc, char **argv ) 
{
	time_t now;

	/* Validate arguments */
	if ( argc != 1 )
		usage ( "argument count" );

	/* Initialize watchdog client */
	cswdog_initialize ( );

	/* Perform an initial watchdog reset */
	cswdog_reset_watchdog ( );

	/* Open the I2C bus */
	barod_sensor_bus = csi2c_open_bus ( 1 );

	/* Initialize the barometer sensor */
	b_initialize ( );

	/* Initialize the IPC library */
	csipc_set_program ( "barod" );	

	/* Create IPC server */
	barod_ipc_server = csipc_create_server ( "barod", 
						 sizeof( csproto_baro_t ),
						 10 );

	/* Set buffer pointer */
	barod_packet = ( csproto_baro_t * ) barod_ipc_server->pl_buffer;

	/* Enter main loop */	
	for ( ; ; ) {

		/* Grab timestamp */
		time ( &now );
	
		/* Process new sensor data */
		b_process ( );

		/* Log temperature */
		cs_log( LOG_DEBUG, 
			"T: \t%f *C\t p: \t%f mbar", 
			b_temperature, 
			b_pressure );

		/* Encode new data */
		barod_packet->timestamp	  = now;
		barod_packet->pressure	  = b_pressure;
		barod_packet->temperature = b_temperature;

		/* Process IPC */
		csipc_server_process ( barod_ipc_server );

		/* Reset watchdog timer */		
		cswdog_reset_watchdog ( ) ;

		/* Wait for 50 milliseconds */
		usleep ( 50000 );		
	
	}


}
