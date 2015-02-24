/* 
 * This file is part of barod.
 *
 * barod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * barod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with barod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	24/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need the libhab I2C API
 */
#include <hab/csi2c.h>

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

/*
 * We need the register map for the barometer
 */
#include "ms5607.h"

/*
 * We need the barod globals header
 */
#include "baro.h"

uint16_t b_read_mem ( uint8_t addr )
{
	uint8_t		buffer[2];
	int 		status;

	/* Do an I2C read on the register */
	status = csi2c_read_register(	barod_sensor_bus, 
					MS5607_ADDR, 
					MS5607_PROM_READ(addr),
					buffer,
					2 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to read barometer PROM address 0x%x: %i(%s)",
		 (int) addr,
		 errno,
		 strerror ( errno ) );

	return (buffer[0] << 8) | buffer[1];

}

uint32_t b_read_adc ( void )
{
	uint8_t		buffer[3];
	int 		status;

	/* Do an I2C read on the register */
	status = csi2c_read_register(	barod_sensor_bus, 
					MS5607_ADDR, 
					MS5607_ADC_READ,
					buffer,
					3 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to read barometer ADC: %i(%s)",
		 errno,
		 strerror ( errno ) );

	return (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];

}

void b_reset ( void )
{
	int 		status;

	/* Write the RESET command to the barometer */
	status = csi2c_write_register(	barod_sensor_bus, 
					MS5607_ADDR, 
					MS5607_RESET,
					NULL,
					0 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to reset barometer: %i(%s)",
		 errno,
		 strerror ( errno ) );

	/* Wait for the barometer to boot */
	usleep(4000);

}

void b_convert ( uint8_t command )
{
	int 		status;

	/* Write the CONVERT command to the barometer */
	status = csi2c_write_register(	barod_sensor_bus, 
					MS5607_ADDR, 
					command,
					NULL,
					0 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to reset barometer: %i(%s)",
		 errno,
		 strerror ( errno ) );

	/* Wait for the barometer to convert the value */
	usleep(10000);

}

void b_initialize ( void )
{

}

void g_process ( void )
{

}
