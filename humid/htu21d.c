/* 
 * This file is part of humid.
 *
 * humid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * humid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with humid.  If not, see <http://www.gnu.org/licenses/>.
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
 * We need the register map for the humidity sensor
 */
#include "htu21d.h"

/*
 * We need the humid globals header
 */
#include "humidity.h"

/* Current humidity in RH% */
double	h_humidity;

/* Current temperature in degrees Celsius */
double	h_temperature;

uint8_t h_read_reg ( void )
{
	uint8_t		buffer[1];
	int 		status;

	/* Do an I2C read on the register */
	status = csi2c_read_register(	humid_sensor_bus, 
					HTU21D_ADDR, 
					HTU21D_READ_UREG,
					buffer,
					1 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to read humidity sensor user register: %i(%s)",
		 errno,
		 strerror ( errno ) );

	return buffer[0];

}

void h_write_reg ( uint8_t value )
{
	uint8_t		buffer[1];
	int 		status;

	buffer[0] = value;

	/* Do an I2C read on the register */
	status = csi2c_read_register(	humid_sensor_bus, 
					HTU21D_ADDR, 
					HTU21D_WRITE_UREG,
					buffer,
					1 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to write humidity sensor user register: %i(%s)",
		 errno,
		 strerror ( errno ) );

}

uint32_t h_read_adc ( void )
{
	uint8_t		buffer[3];
	int 		status;

	/* Do an I2C read on the register */
	status = csi2c_read_device(	humid_sensor_bus, 
					HTU21D_ADDR, 
					buffer,
					3 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to read humidity sensor ADC: %i(%s)",
		 errno,
		 strerror ( errno ) );

	//TODO: Verify CRC

	return (buffer[0] << 8) | buffer[1];

}

void h_reset ( void )
{
	int 		status;

	/* Write the RESET command to the humidity sensor */
	status = csi2c_write_register(	humid_sensor_bus, 
					HTU21D_ADDR, 
					HTU21D_RESET,
					NULL,
					0 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to reset humidity sensor: %i(%s)",
		 errno,
		 strerror ( errno ) );

	/* Wait for the humidity sensor to boot */
	usleep(20000);

}

void h_convert ( uint8_t command )
{
	int 		status;

	/* Write the CONVERT command to the humidity sensor */
	status = csi2c_write_register(	humid_sensor_bus, 
					HTU21D_ADDR, 
					command,
					NULL,
					0 );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to measure humidity sensor value: %i(%s)",
		 errno,
		 strerror ( errno ) );

	/* Wait for the humidity sensor to convert the value */
	usleep(60000);

}

void h_initialize ( void )
{

	/* Reset the sensor chip */
	h_reset ( );

}

void h_process ( void )
{
	
	uint32_t m_temp;
	uint32_t m_humid;
	/* Start conversion of temperature */
	h_convert ( HTU21D_TRIG_TEMP_RB );

	/* Read result */
	m_temp = h_read_adc() & 0xFFFC;

	/* Start conversion of humidity */
	h_convert ( HTU21D_TRIG_TEMP_RB );

	/* Read result */
	m_humid = h_read_adc() & 0xFFFC;

	/* Calculate temperature and pressure as double */
	h_temperature = (((double) m_temp ) / 65536.0) * 175.72 - 46.85;
	h_humidity    = (((double) m_humid) / 65536.0) * 125.0  -  6.0;

	/* Log temperature */
	cs_log( LOG_DEBUG, 
		"T: \t%f *C\t RH: \t%f %", 
		h_temperature, 
		h_humidity );
 
}
