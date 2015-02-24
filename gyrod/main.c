/* 
 * This file is part of gyrod.
 *
 * gyrod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * gyrod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with gyrod.  If not, see <http://www.gnu.org/licenses/>.
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
 * We need the gyroscope functions
 */
#include "gyro.h"

csi2c_bus_t	*gyrod_sensor_bus;
cs_srv_t	*gyrod_ipc_server;
csproto_gyro_t  *gyrod_packet;

void usage ( const char *reason )
{
	fprintf( stderr, 
		 "Syntax error: %s\nUsage: gyrod\n",
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
	gyrod_sensor_bus = csi2c_open_bus ( 1 );

	/* Initialize the gyroscope sensor */
	g_initialize ( );

	/* Initialize the IPC library */
	csipc_set_program ( "gyrod" );	

	/* Create IPC server */
	gyrod_ipc_server = csipc_create_server ( "gyrod", 
						 sizeof( csproto_gyro_t ),
						 10 );

	/* Set buffer pointer */
	gyrod_packet = ( csproto_gyro_t * ) gyrod_ipc_server->pl_buffer;

	/* Enter main loop */	
	for ( ; ; ) {

		/* Grab timestamp */
		time ( &now );
	
		/* Process new sensor data */
		g_process ( );

		/* Encode new data */
		gyrod_packet->timestamp	= now;
		gyrod_packet->range 	= g_scale * 32768.0;
		gyrod_packet->rate_x 	= g_rate_x;
		gyrod_packet->rate_y 	= g_rate_y;
		gyrod_packet->rate_z 	= g_rate_z; 

		/* Process IPC */
		csipc_server_process ( gyrod_ipc_server );

		/* Reset watchdog timer */		
		cswdog_reset_watchdog ( ) ;

		/* Wait for 50 milliseconds */
		usleep ( 50000 );		
	
	}


}
