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

/* Pressure sensitivity SENSt1 */
 int64_t b_psens_t1;

/* Pressure offset OFFt1 */
 int64_t b_poff_t1;

/* Temperature coefficient of pressure sensitivity TCS */
 int64_t b_tc_psens;

/* Temperature coefficient of pressure offset TCO */
 int64_t b_tc_poff;

/* Reference temperature Tref */
 int32_t b_tref;

/* Temperature coefficient of temperature reading TEMPSENS */
 int64_t b_tsens;

/* Current barometric pressure in millibar */
double	b_pressure;

/* Current temperature in degrees Celsius */
double	b_temperature;

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

uint8_t	b_crc4 ( uint16_t prom[] )
{
	int 		ctr;
	uint16_t	n_rem;
	uint16_t	crc_read;
	uint8_t		n_bit;
	
	n_rem = 0x00;

	crc_read = prom[ MS5607_M_CRC ];

	prom [ MS5607_M_CRC ] &= 0xFF00;

	for ( ctr = 0; ctr < 16; ctr++ ) {
		n_rem ^= ( ctr & 1 ) ? 
			( ( uint16_t ) ( prom[ ctr >> 1 ] & 0xFF ) ) : 
			( ( uint16_t ) ( ( prom[ ctr >> 1 ] & 0xFF00 ) >> 8 ) );
		
		for ( n_bit = 8; n_bit > 0; n_bit-- ) {
			if ( n_rem & 0x8000 )
				n_rem = ( n_rem << 1 ) ^ 0x3000;
			else
				n_rem = ( n_rem << 1 );
		}
	}

	n_rem = 0x0F & ( n_rem >> 12);

	prom[ MS5607_M_CRC ] = crc_read;

	return n_rem;

}

void b_load_cal ( void )
{
	int 	 ctr;
	uint8_t	 crc, crc_ex;
	uint16_t prom[8];

	/* Read the PROM */
	for ( ctr = 0; ctr < 8; ctr++ )
		prom[ ctr ] = b_read_mem ( ctr * 2 );

	/* Determine the expected CRC */
	crc_ex = (uint8_t) ( prom[ MS5607_M_CRC ] & 0x000F );

	/* Calculate the CRC */
	crc = b_crc4 ( prom );

	/* Verify it */
	cserror( crc == crc_ex, 
		 LOG_ERROR, 
		 "Barometer calibration data CRC mismatch: 0x%x expected: 0x%x",
		 (int) crc,
		 (int) crc_ex);

	/* Load values */
	b_psens_t1 = (int64_t) ( ((int32_t) prom[ MS5607_M_C1 ] ) & 0x0000FFFF);
	b_poff_t1  = (int64_t) ( ((int32_t) prom[ MS5607_M_C2 ] ) & 0x0000FFFF);
	b_tc_psens = (int64_t) ( ((int32_t) prom[ MS5607_M_C3 ] ) & 0x0000FFFF);
	b_tc_poff  = (int64_t) ( ((int32_t) prom[ MS5607_M_C4 ] ) & 0x0000FFFF);
	b_tref     = ( ( int32_t ) prom[ MS5607_M_C5 ] ) & 0x0000FFFF;
	b_tsens    = (int64_t) ( ((int32_t) prom[ MS5607_M_C6 ]) & 0x0000FFFF);

	/* Log */
	cs_log ( LOG_INFO, "Barometer calibration data loaded.\n");

}

void b_initialize ( void )
{

	/* Reset the barometer chip */
	b_reset ( );
	
	/* Load calibration data */
	b_load_cal ( );

}

void b_process ( void )
{
	 int32_t	m_temp;
	 int32_t	m_pres;
	 int64_t	d_temp;
	 int64_t	b_temp;
	 int64_t	p_off;
	 int64_t	p_sens;
	 int64_t	c_off  = 0;
	 int64_t	c_sens = 0;
	 int64_t	c_temp = 0;
	 int64_t	b_pres;

	/* Get measurements */
	b_convert ( MS5607_CV_BARO_4K );
	m_pres = (int32_t) b_read_adc ( );

	b_convert ( MS5607_CV_TEMP_4K );
	m_temp = (int32_t) b_read_adc ( );

	/* Calculate temperature */
	d_temp = m_temp - ( b_tref << 8 ) ;
	b_temp = 2000 + ((d_temp * b_tsens) >> (int64_t) 23);

	/* Calculate pressure offset and sensitivity */
	p_off  = ( b_poff_t1 << 17 )  + ( ( b_tc_poff  * d_temp ) >> 6 );
	p_sens = ( b_psens_t1 << 16 ) + ( ( b_tc_psens * d_temp ) >> 7 );
	
	/* Handle nonlinearity */
	if ( b_temp < 2000 ) {
		/* Low temperature ( < 20*C ) */
		c_temp = ( d_temp * d_temp ) >> 31;
		c_off  = ( 61 * ( b_temp - 2000 ) * ( b_temp - 2000 ) ) >> 4;
		c_sens =    2 * ( b_temp - 2000 ) * ( b_temp - 2000 );
	} 
	if ( b_temp < -1500 ) {
		/* Very low temperature ( < -15*C ) */
		c_off  +=  15 * ( b_temp + 1500 ) * ( b_temp + 1500 );
		c_sens +=   8 * ( b_temp + 1500 ) * ( b_temp + 1500 );
	}

	/* Apply second order factors */
	p_off  -= c_off;
	p_sens -= c_sens;
	b_temp -= c_temp;

	/* Calculate pressure */
	b_pres  = ( ( ( m_pres * p_sens ) >> 21 ) - p_off ) >> 15;

	/* Calculate temperature and pressure as double */
	b_temperature = ( (double) b_temp ) / 100.0;
	b_pressure    = ( (double) b_pres ) / 100.0;
 
}
