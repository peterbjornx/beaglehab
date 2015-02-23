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
 * We need the register map for the IMU
 */
#include "lsm9ds0.h"

/*
 * We need the imud globals header
 */
#include "imud.h"

uint8_t g_read_reg ( uint8_t reg_addr )
{
	uint8_t	value;
	int status;

	/* Do an I2C read on the register */
	status = csi2c_read_register(	imud_sensor_bus, 
					LSM9DS0_G_ADDR, 
					reg_addr,
					&value,
					sizeof( uint8_t ) );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to read gyroscope register 0x%x: %i(%s)",
		 (int) reg_addr,
		 errno,
		 strerror ( errno ) );

	return value;

}

void g_initialize ( void )
{
	uint8_t magic;
	magic = g_read_reg ( LSM9DS0_WHO_AM_I_G );
	cs_log(LOG_INFO, "Gyroscope reported magic 0x%x\n", magic);
}
