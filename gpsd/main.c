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
#include "gpsd.h"
#include <hab/cslog.h>
#include <hab/csdata.h>
#include <string.h>
#include <errno.h>
#include <nmea/nmea.h>
#include <math.h>

/*
 * We need hab/csipc for the libhab IPC system
 */
#include <hab/csipc.h>

/*
 * We need hab/cswdog for the watchdog reset function
 */
#include <hab/cswdog.h>

void nmealib_trace(const char *str, int str_size)
{
	char *buf = malloc(str_size + 1);
	buf[str_size] = 0;
	memcpy(buf, str, str_size);
	cs_log(LOG_TRACE, "NMEALIB: %s\n", buf);
	free(buf);
}

void nmealib_error(const char *str, int str_size)
{
	char *buf = malloc(str_size + 1);
	buf[str_size] = 0;
	memcpy(buf, str, str_size);
	cs_log(LOG_ERROR, "NMEALIB: %s\n", buf);
	free(buf);
}

nmeaINFO info;
nmeaPARSER parser;

char	sentence_buffer[128];

cs_srv_t	*gpsd_ipc_server;
csproto_gps_t	*gpsd_packet;

void usage ( const char *reason )
{
	fprintf( stderr, 
		 "Syntax error: %s\nUsage: gpsd\n",
		 reason );
	exit ( EXIT_FAILURE );
}

int main( int argc, char **argv ) 
{
	time_t now;
	FILE *gps;
	int size;

	/* Validate arguments */
	if ( argc != 1 )
		usage ( "argument count" );

	/* Initialize watchdog client */
	cswdog_initialize ( );

	/* Perform an initial watchdog reset */
	cswdog_reset_watchdog ( );

	/* Open the gps receiver */
	gps = gps_open("/dev/ttyO2");

	/* Initialize NMEA parser library */
	nmea_property()->trace_func = &nmealib_trace;
	nmea_property()->error_func = &nmealib_error;

	nmea_zero_INFO(&info);
	nmea_parser_init(&parser);

	/* Initialize the IPC library */
	csipc_set_program ( "gpsd" );	

	/* Create IPC server */
	gpsd_ipc_server = csipc_create_server ( "gpsd", 
						 sizeof( csproto_gps_t ),
						 10 );

	/* Set buffer pointer */
	gpsd_packet = ( csproto_gps_t * ) gpsd_ipc_server->pl_buffer;


	for (;;) {

		/* Read next sentence from the GPS receiver */
		size = gps_read_sentence(gps, sentence_buffer, 128);

		/* Parse it using NMEALIB */
		nmea_parse(&parser, sentence_buffer, size, &info);

		/* Log the data */
		cs_log(LOG_DEBUG, " lat: %f lon: %f alt:%f vel: %f head:%f\n",
			info.lat, 
			info.lon, 
			info.elv, 
			info.speed, 
			info.direction);

		/* Update IPC data */
		gpsd_packet->lat_degrees = floor(info.lat / 100.0);
		gpsd_packet->lat_minutes = fmod(info.lat,100.0);
		gpsd_packet->long_degrees = floor(info.lon / 100.0);
		gpsd_packet->long_minutes = fmod(info.lon,100.0);
		gpsd_packet->velocity = info.speed / 3.6;
		gpsd_packet->altitude = info.elv;
		gpsd_packet->heading = info.direction;

		/* Process IPC */
		csipc_server_process ( gpsd_ipc_server );

		/* Reset watchdog timer */		
		cswdog_reset_watchdog ( ) ;
		
	}
}
