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
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __radiod_h__
#define __radiod_h__

#include "csproto.h"
#include <hab/csdata.h>

FILE *radio_open(const char *path);

void radio_send(FILE *radio, csproto_packet_t *packet);

void radio_printf(FILE *radio, const char *format, ...) ;

void nav_telemetry_update ( csproto_telemetry_t *telemetry );

void nav_initialize ( void );

#endif
