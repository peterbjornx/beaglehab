/* 
 * This file is part of imud.
 *
 * imud is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * imud is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with imud.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	23/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __imud_h__
#define __imud_h__

/*
 * We need the libhab I2C API
 */
#include <hab/csi2c.h>

extern csi2c_bus_t *imud_sensor_bus;

void g_initialize ( void );

#endif
