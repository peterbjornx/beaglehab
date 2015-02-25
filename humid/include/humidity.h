/* 
 * This file is part of humid.
 *
 * humid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * humid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with humid.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	25/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __humidity_h__
#define __humidity_h__

/*
 * We need the libhab I2C API
 */
#include <hab/csi2c.h>

/* Current humidity in RH% */
extern double		 h_humidity;

/* Current temperature in degrees Celsius */
extern double		 h_temperature;

extern csi2c_bus_t 	*humid_sensor_bus;

void h_initialize ( void );

void h_process ( void );

#endif
