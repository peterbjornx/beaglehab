/* 
 * This file is part of libhab.
 *
 * libhab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * libhab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with libhab.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	06/01/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __csdata_h__
#define __csdata_h__

/**
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

/**
 * We need stddef for the size_t type definition.
 */
#include <stddef.h>

/**
 * These definitions specify the conversion factors used to convert the 
 * floating point telemetry data to the fixed-point transmission format
 */

#define MINUTE_SCALE		(60.0 / 65536.0)
#define HEADING_SCALE		(360.0 / 65536.0)
#define VELOCITY_SCALE		(1.0 / 20.0)
#define ALTITUDE_SCALE		(1.0 / 256.0)
#define ACCELERATION_SCALE	(1000.0 / 32768.0)

typedef struct {
	double	lat_degrees;
	double	lat_minutes;
	double	long_degrees;
	double	long_minutes;
	double	velocity;
	double	heading;
	double	altitude;
	double	acc_x, acc_y, acc_z;
} csproto_telemetry_t;

typedef struct {
	double lat_degrees;
	double lat_minutes;
	double long_degrees;
	double long_minutes;
	double velocity;
	double heading;
	double altitude;
} csproto_gps_t;

#endif
