/* 
 * This file is part of bmed.
 *
 * bmed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * bmed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with barod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	13/07/2019
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need the libhab SPI API
 */
#include <hab/csspi.h>

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
 * We need the BME functions
 */
#include "bme.h"

csspi_dev_t			*bmed_sensor_dev;
cs_srv_t			*barod_ipc_server;
csproto_baro_t  	*barod_packet;
cs_srv_t			*humid_ipc_server;
csproto_humidity_t  *humid_packet;

void usage ( const char *reason )
{
	fprintf( stderr, 
		 "Syntax error: %s\nUsage: bmed\n",
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

	/* Open the SPI device */
	bmed_sensor_dev = csspi_open_dev ( 1, 0, SPI_MODE_0, 0  );

	/* Initialize the BME sensor */
	bme_initialize ( );

	/* Initialize the IPC library */
	csipc_set_program ( "bmed" );	

	/* Create barod IPC server */
	barod_ipc_server = csipc_create_server ( "barod", 
						 sizeof( csproto_baro_t ),
						 10 );

	/* Create humid IPC server */
	humid_ipc_server = csipc_create_server ( "humid", 
						 sizeof( csproto_humidity_t ),
						 10 );

	/* Set buffer pointers */
	barod_packet = ( csproto_baro_t * )     barod_ipc_server->pl_buffer;
	humid_packet = ( csproto_humidity_t * ) humid_ipc_server->pl_buffer;

	/* Enter main loop */	
	for ( ; ; ) {
	
		/* Process new sensor data */
		if ( !bme_process ( ) )
			continue;

		/* Log temperature */
		cs_log( LOG_DEBUG, 
			"T: \t%f *C\t p: \t%f mbar RH: \t%f", 
			bme_temperature, 
			bme_pressure,
			bme_humidity
			);

		/* Encode new data */
		barod_packet->timestamp	  = now;
		barod_packet->pressure	  = bme_pressure;
		barod_packet->temperature = bme_temperature;
		
		humid_packet->timestamp	  = now;
		humid_packet->humidity    = bme_humidity;
	    humid_packet->temperature = bme_temperature;

		/* Process IPC */
		csipc_server_process ( barod_ipc_server );
		csipc_server_process ( humid_ipc_server );

		/* Reset watchdog timer */		
		cswdog_reset_watchdog ( ) ;

		/* Grab timestamp */
		time ( &now );

		/* Wait for specified time */
		usleep ( bme_loop_delay * 1000 );		
	
	}


}
