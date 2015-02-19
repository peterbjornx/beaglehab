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
 * Created:	06/01/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */


/**
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

#include "csproto.h"

void	csproto_encode_telemetry(csproto_packet_t *packet, const csproto_telemetry_t *telemetry)
{
	uint32_t alt_int;
	packet->lat_deg  = (uint8_t) (128 + (int) telemetry->lat_degrees);
	packet->lat_min  = (uint16_t) (telemetry->lat_minutes / MINUTE_SCALE);
	packet->long_deg = (uint8_t) (128 + (int) telemetry->long_degrees);
	packet->long_min = (uint16_t) (telemetry->long_minutes / MINUTE_SCALE);
	packet->velocity = (uint16_t) (telemetry->velocity / VELOCITY_SCALE);
	packet->heading  = (uint16_t) (telemetry->heading / HEADING_SCALE);
	alt_int		 = (uint32_t) (telemetry->altitude / ALTITUDE_SCALE);
	packet->altitude_msb = (uint8_t) ((alt_int >> 16) & 0xFF);
	packet->altitude_lsw = (uint16_t) (alt_int & 0xFFFF);
	packet->acc_x	 = (uint16_t) (32768 + telemetry->acc_x / ACCELERATION_SCALE);
	packet->acc_y	 = (uint16_t) (32768 + telemetry->acc_y / ACCELERATION_SCALE);
	packet->acc_z	 = (uint16_t) (32768 + telemetry->acc_z / ACCELERATION_SCALE);
}

