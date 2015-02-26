/* 
	This file is part of radiod.

	radiod is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	radiod is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with radiod.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "radiod.h"
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <math.h>
#include <hab/cslog.h>
#include <hab/cswdog.h>
#include <hab/csdata.h>
#include <hab/csipc.h>

csproto_packet_t 	pakketje;
csproto_telemetry_t	telemetry;

#define PAYLOAD_COUNT	3

radio_payload_enc_t	payloadenc[] = {imu_payload_enc_gyr,
					imu_payload_enc_mag,
					ath_payload_enc};

int main( int argc, char **argv ) 
{
	int payload_ctr;
	FILE *radio = radio_open("/dev/ttyO4");

	csipc_set_program( "radiod" );

	cswdog_initialize ( ) ;
	
	nav_initialize ( );

	imu_initialize ( );

	telemetry.lat_degrees  = 52.0;
	telemetry.lat_minutes  = 16.0 + 24.523/60.0;
	telemetry.long_degrees =  6.0;
	telemetry.long_minutes = 47.0 +  52.161/60.0;
	telemetry.altitude     = 1.50;
	telemetry.velocity     = 42;
	telemetry.heading      = 56.78;
	telemetry.acc_x	       = 999.567;
	telemetry.acc_y	       = -999.566;
	telemetry.acc_z	       = 0;

	for (;;) {
	
/* Telemetry main packet updaters go here */

		nav_telemetry_update ( &telemetry );

		imu_telemetry_update ( &telemetry );

/* -------------------------------------- */

		csproto_encode_telemetry(&pakketje, &telemetry);

		pakketje.payload_id = 
			payloadenc[ payload_ctr++ ]( pakketje.payload_data );

		payload_ctr %= PAYLOAD_COUNT;

		csproto_prepare(&pakketje);

		radio_send(radio, &pakketje);

		cs_log(LOG_TRACE, "Transmitting packet with telemetry data: LN:%f LT:%f\n", telemetry.lat_degrees, telemetry.long_degrees);

		cswdog_reset_watchdog ( );
		
	}
}
