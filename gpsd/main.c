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
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>

csproto_gps_t *gpsblock;

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

int main( int argc, char **argv ) 
{
	/* Not really a daemon... */
	int id = 0, size;

	FILE *gps = gps_open("/dev/ttyO2");

	nmea_property()->trace_func = &nmealib_trace;
	nmea_property()->error_func = &nmealib_error;

	nmea_zero_INFO(&info);
	nmea_parser_init(&parser);

	for (;;) {

		size = gps_read_sentence(gps, sentence_buffer, 128);

		nmea_parse(&parser, sentence_buffer, size, &info);

		cs_log(LOG_INFO, " lat: %f lon: %f alt:%f vel: %f head:%f\n", info.lat, info.lon, info.elv, info.speed, info.direction);

		gpsblock->lat_degrees = floor(info.lat / 100.0);
		gpsblock->lat_minutes = fmod(info.lat,100.0);
		gpsblock->long_degrees = floor(info.lon / 100.0);
		gpsblock->long_minutes = fmod(info.lon,100.0);
		gpsblock->velocity = info.speed / 3.6;
		gpsblock->altitude = info.elv;
		gpsblock->heading = info.direction;
		
	}
}
