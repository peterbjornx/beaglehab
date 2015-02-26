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
 * Created:	26/02/2015
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
 * We need csproto for packet structures
 */
#include "csproto.h"

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

cs_chan_t		*baro_chan;
cs_chan_t		*humid_chan;

csproto_baro_t		*baro_data;
csproto_humidity_t	*humid_data;

void ath_initialize ( void )
{
	baro_chan = csipc_open_channel( "barod", sizeof( csproto_baro_t ), 10);
	baro_data = ( csproto_baro_t * ) baro_chan->pl_buffer;
	humid_chan = csipc_open_channel( "humid", sizeof( csproto_humidity_t ), 10);
	humid_data = ( csproto_humidity_t * ) humid_chan->pl_buffer;
}

uint8_t ath_payload_enc ( uint8_t *payload )
{
	csproto_payload_ath_t	*pkt = (csproto_payload_ath_t *) payload;
	csipc_client_process ( baro_chan );
	csipc_client_process ( humid_chan );
	baro_data = ( csproto_baro_t * ) baro_chan->pl_buffer;
	humid_data = ( csproto_humidity_t * ) humid_chan->pl_buffer;

	pkt->pressure = (uint16_t) (baro_data->pressure / CS_PRESSURE_SCALE);
	pkt->temperature = (uint16_t) ((baro_data->temperature 
			+ CS_TEMPERATURE_OFFSET) / CS_TEMPERATURE_SCALE);
	pkt->humidity = (uint16_t) ((humid_data->humidity
			+ CS_HUMIDITY_OFFSET) / CS_HUMIDITY_SCALE);

	return CS_PAYLOAD_ATH;
}
