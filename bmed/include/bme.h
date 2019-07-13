/* 
 * This file is part of bmed.
 *
 * bmed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * bmed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with barod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	13/07/2019
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __bme_h__
#define __bme_h__

/*
 * We need the libhab SPI API
 */
#include <hab/csspi.h>

/* Current barometric pressure in millibar */
extern double		 bme_pressure;

/* Current temperature in degrees Celsius */
extern double		 bme_temperature;

/* Current relative humidity in percent */
extern double		 bme_humidity;

/* Current VOC concentration  */
extern double		 bme_temperature;

extern csspi_dev_t 	*bmed_sensor_dev;

extern int           bme_loop_delay;

void bme_initialize ( void );

int bme_process ( void );

#endif
