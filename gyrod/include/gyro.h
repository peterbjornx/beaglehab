/* 
 * This file is part of gyrod.
 *
 * gyrod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * gyrod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with gyrod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	23/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __gyro_h__
#define __gyro_h__

/*
 * We need the libhab I2C API
 */
#include <hab/csi2c.h>

extern csi2c_bus_t 	*gyrod_sensor_bus;

extern double		 g_scale;
extern double		 g_offset_x;
extern double		 g_offset_y;
extern double		 g_offset_z;

void g_initialize ( void );

void g_set_scale ( uint8_t scale );

void g_calibrate ( void );

void g_process ( void );

#endif
