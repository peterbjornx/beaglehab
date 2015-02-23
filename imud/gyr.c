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
 * We need string to generate friendly names for errors
 */
#include <usleep.h>

/*
 * We need the register map for the IMU
 */
#include "lsm9ds0.h"

/*
 * We need the imud globals header
 */
#include "imud.h"

volatile lsm9ds0_output_t	g_output;

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

void g_poll_output( void )
{
	int status;

	/* Do an I2C read on the registers */
	status = csi2c_read_register(	imud_sensor_bus, 
					LSM9DS0_G_ADDR, 
					LSM9DS0_OUT_X_L_G | 0x80,
					&g_output,
					sizeof( lsm9ds0_output_t ) );

	g_read_reg(LSM9DS0_OUT_Z_H_G );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to read gyroscope output: %i(%s)",
		 errno,
		 strerror ( errno ) );

	cs_log(LOG_INFO, "Gyroscope measurements: [%i;\t%i;\t%i]", 
			(int)g_output.x,
			(int)g_output.y,
			(int)g_output.z);

}

void g_write_reg ( uint8_t reg_addr, uint8_t value )
{
	int status;

	/* Do an I2C write on the register */
	status = csi2c_write_register(	imud_sensor_bus, 
					LSM9DS0_G_ADDR, 
					reg_addr,
					&value,
					sizeof( uint8_t ) );

	cserror( status >= 0,
		 LOG_ERROR,
		 "Failed to write gyroscope register 0x%x: %i(%s)",
		 (int) reg_addr,
		 errno,
		 strerror ( errno ) );

	usleep(10000);

}

void g_initialize ( void )
{
	uint8_t temp;

	/* Tell the gyroscope to reboot */
	g_write_reg ( 	LSM9DS0_CTRL_REG5_G, 
		  	LSM9DS0_CR5G_BOOT );

	/* Check if we can talk to the gyroscope */
	temp = g_read_reg ( LSM9DS0_WHO_AM_I_G );
	cserror(	temp == LSM9DS0_G_MAGIC, 
			LOG_ERROR,
			"Invalid gyroscope magic: 0x%x", 
			temp );

	/* Set data rate to 95 Hz, bandwith to 12.5, enable X, Y, Z channels */
	g_write_reg ( 	LSM9DS0_CTRL_REG1_G, 
		  	LSM9DS0_CR1G_DR_95HZ | 
			LSM9DS0_CR1G_BW0 |
			LSM9DS0_CR1G_PD  |
			LSM9DS0_CR1G_XEN |
			LSM9DS0_CR1G_YEN |
			LSM9DS0_CR1G_ZEN);

	/* Set highpass filter mode to normal, cutoff frequency to 7.2 Hz */
	g_write_reg ( 	LSM9DS0_CTRL_REG2_G, 
		  	LSM9DS0_CR2G_HP_RNORMAL | 
			LSM9DS0_CR2G_HPCF( 0 ) );

	/* Disable digital outputs besides the data bus */
	g_write_reg ( 	LSM9DS0_CTRL_REG3_G, 0 );

	/* Set block update mode, little endian mode and set the scale to 245 */
	g_write_reg ( 	LSM9DS0_CTRL_REG4_G, 
			LSM9DS0_CR4G_BLOCK_UPD |
			LSM9DS0_CR4G_SCALE_245 );

	/* Enable FIFO and highpass filter, set ADC input to HPF */
	g_write_reg ( 	LSM9DS0_CTRL_REG5_G, 
			LSM9DS0_CR5G_FIFO_EN |
			LSM9DS0_CR5G_HPF_EN |
			LSM9DS0_CR5G_OS_HPF );

	/* Set FIFO to stream mode */
	g_write_reg ( 	LSM9DS0_FIFO_CTRL_REG_G, 
			LSM9DS0_CRFG_FM_STREAM |
			LSM9DS0_CRFG_WTM(7) );

	
}
