/* 
 * This file is part of humid.
 *
 * humid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * humid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with humid.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	25/02/2015
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
 * We need the humidity sensor functions
 */
#include "humidity.h"

csi2c_bus_t		*humid_sensor_bus;
cs_srv_t		*humid_ipc_server;
csproto_humidity_t	*humid_packet;

void usage ( const char *reason )
{
	fprintf( stderr, 
		 "Syntax error: %s\nUsage: humid\n",
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
	humid_sensor_bus = csi2c_open_bus ( 1 );

	/* Initialize the humidity sensor */
	h_initialize ( );

	/* Initialize the IPC library */
	csipc_set_program ( "humid" );	

	/* Create IPC server */
	humid_ipc_server = csipc_create_server ( "humid", 
						 sizeof( csproto_humidity_t ),
						 10 );

	/* Set buffer pointer */
	humid_packet = ( csproto_humidity_t * ) humid_ipc_server->pl_buffer;

	/* Enter main loop */	
	for ( ; ; ) {

		/* Grab timestamp */
		time ( &now );
	
		/* Process new sensor data */
		h_process ( );

		/* Encode new data */
		humid_packet->timestamp	  = now;
		humid_packet->humidity	  = h_humidity;
		humid_packet->temperature = h_temperature;

		/* Process IPC */
		csipc_server_process ( humid_ipc_server );

		/* Reset watchdog timer */		
		cswdog_reset_watchdog ( ) ;

		/* Wait for 100 milliseconds */
		usleep ( 1000000 );		
	
	}


}
