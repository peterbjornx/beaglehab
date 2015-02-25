/* 
 * This file is part of radiod.
 *
 * radiod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * radiod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with radiod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	25/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need the libhab ipc API
 */
#include <hab/csipc.h>

/*
 * We need hab/csdata for ipc format
 */
#include <hab/csdata.h>

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
 * We need string to generate friendly names for errors
 */
#include <unistd.h>

cs_chan_t	*gps_chan;

csproto_gps_t	*gps_data;

void nav_initialize ( void )
{
	gps_chan = csipc_open_channel( "gpsd", sizeof( csproto_gps_t ), 10);
	gps_data = ( csproto_gps_t * ) gps_chan->pl_buffer;
}

void nav_telemetry_update ( csproto_telemetry_t *telemetry )
{
	csipc_client_process ( gps_chan );

	telemetry->lat_degrees	= gps_data->lat_degrees;
	telemetry->lat_minutes	= gps_data->lat_minutes;
	telemetry->long_degrees	= gps_data->long_degrees;
	telemetry->long_minutes	= gps_data->long_minutes;
	telemetry->velocity	= gps_data->velocity;
	telemetry->heading	= gps_data->heading;
	telemetry->altitude	= gps_data->altitude;
}
