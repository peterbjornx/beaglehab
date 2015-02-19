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
 * Protocol definition for the CanSat downlink
 * Created:	06/01/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __csproto_h__
#define __csproto_h__

/**
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

/**
 * We need stddef for the size_t type definition.
 */
#include <stddef.h>

/**
 * These values will generate an easily recognizable pattern in the radio
 * transmission and will be used by the receiver to detect the start of a
 * frame. 
 */

#define CS_PREAMBLE_L	(0xAA)
#define CS_PREAMBLE_H	(0x55)

/**
 * 
 */
#include <hab/csdata.h> 

/**
 * This structure describes the layout of a downlink packet, it is exactly
 * 30 bytes in length and will take exactly one second to transmit over the
 * 300-8N1 downlink
 */

 struct cspd {
	uint8_t		preamble_h;
	uint8_t		preamble_l;
	uint8_t		lat_deg;
	uint16_t	lat_min;
	uint8_t		long_deg;
	uint16_t	long_min;
	uint16_t	velocity;
	uint16_t	heading;
	uint8_t		altitude_msb;
	uint16_t	altitude_lsw;
	uint16_t	acc_x;
	uint16_t	acc_y;
	uint16_t	acc_z;
	uint8_t		payload_id;
	uint8_t		payload_data[6];
	uint16_t	checksum;
} __attribute__((packed));

typedef struct cspd csproto_packet_t;

void		csproto_prepare	( csproto_packet_t *packet );

uint16_t	csproto_checksum( void *data, size_t length );

void		csproto_encode_telemetry(csproto_packet_t *packet, const csproto_telemetry_t *telemetry);

#endif
