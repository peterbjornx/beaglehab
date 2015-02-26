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

cs_chan_t	*gyro_chan;
cs_chan_t	*acmm_chan;

csproto_gyro_t	*gyro_data;
csproto_acmm_t	*acmm_data;

void imu_initialize ( void )
{
	gyro_chan = csipc_open_channel( "gyrod", sizeof( csproto_gyro_t ), 10);
	gyro_data = ( csproto_gyro_t * ) gyro_chan->pl_buffer;
	acmm_chan = csipc_open_channel( "acmmd", sizeof( csproto_acmm_t ), 10);
	acmm_data = ( csproto_acmm_t * ) acmm_chan->pl_buffer;
}

void imu_telemetry_update ( csproto_telemetry_t *telemetry )
{
	csipc_client_process ( acmm_chan );

	telemetry->acc_x = acmm_data->acc_x;
	telemetry->acc_y = acmm_data->acc_y;
	telemetry->acc_z = acmm_data->acc_z;
}

uint8_t imu_payload_enc_mag ( uint8_t *payload )
{
	csproto_payload_mag_t	*pkt = (csproto_payload_mag_t *) payload;
	csipc_client_process ( acmm_chan );

	pkt->mag_x = (uint16_t)
		((acmm_data->mag_x / CS_MAGNETIC_SCALE) + CS_MAGNETIC_OFFSET);
	pkt->mag_y = (uint16_t)
		((acmm_data->mag_y / CS_MAGNETIC_SCALE) + CS_MAGNETIC_OFFSET);
	pkt->mag_z = (uint16_t)
		((acmm_data->mag_z / CS_MAGNETIC_SCALE) + CS_MAGNETIC_OFFSET);

	return CS_PAYLOAD_MAG;
}

uint8_t imu_payload_enc_gyr ( uint8_t *payload )
{
	csproto_payload_gyr_t	*pkt = (csproto_payload_gyr_t *) payload;

	csipc_client_process ( gyro_chan );

	pkt->gyr_x = (uint16_t)
		((gyro_data->rate_x / CS_GYROSCOPE_SCALE) + CS_GYROSCOPE_OFFSET);
	pkt->gyr_y = (uint16_t)
		((gyro_data->rate_y / CS_GYROSCOPE_SCALE) + CS_GYROSCOPE_OFFSET);
	pkt->gyr_z = (uint16_t)
		((gyro_data->rate_z / CS_GYROSCOPE_SCALE) + CS_GYROSCOPE_OFFSET);

	return CS_PAYLOAD_GYR;
}

