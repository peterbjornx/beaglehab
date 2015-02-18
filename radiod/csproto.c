/* 
 * This file is part of radiod.
 *
 * radiod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * radiod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with radiod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * We need stdint for the unsigned integer type definitions
 */
#include <stdint.h>

/**
 * We also need the header containing the protocol definitions
 */
#include "csproto.h"

/**
 * Prepares a packet for transmission
 */
void		csproto_prepare	( csproto_packet_t *packet )
{
	packet->preamble_l = CS_PREAMBLE_L;
	packet->preamble_h = CS_PREAMBLE_H;
	packet->checksum = csproto_checksum( packet, sizeof(csproto_packet_t) - sizeof(uint16_t) );
}

/**
 * Calculates the checksum for a block of data
 */
uint16_t	csproto_checksum( void *data, size_t length )
{
	int		ptr;
	uint8_t	*	_chkdt;
	uint16_t	checksum = 1;

	_chkdt = (uint8_t *) data;

	for (ptr = 0; ptr < length; ptr++)
		checksum += _chkdt[ptr];

	return checksum;
}
