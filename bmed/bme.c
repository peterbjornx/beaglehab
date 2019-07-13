/* 
 * This file is part of bmed.
 *
 * bmed is free software: you can redistribute it and/or modify
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
 * Created:	13/07/2019
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need the libhab SPI API
 */
#include <hab/csspi.h>

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
 * We need the BME680 driver library
 */
#include "bme680.h"

/*
 * We need the bmed globals header
 */
#include "bme.h"

int bme_loop_delay;
double bme_pressure;
double bme_humidity;
double bme_temperature;

struct bme680_dev gas_sensor;
struct bme680_field_data data;

void user_delay_ms(uint32_t period);
int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

void lib_init( void )
{
	/* You may assign a chip select identifier to be handled later */
    gas_sensor.dev_id = 0;
    gas_sensor.intf = BME680_SPI_INTF;
    gas_sensor.read = user_spi_read;
    gas_sensor.write = user_spi_write;
    gas_sensor.delay_ms = user_delay_ms;
    /* amb_temp can be set to 25 prior to configuring the gas sensor 
     * or by performing a few temperature readings without operating the gas sensor.
     */
    gas_sensor.amb_temp = 25;

    int8_t rslt = BME680_OK;
    rslt = bme680_init(&gas_sensor);
    
    cserror( rslt == BME680_OK, 
    		  LOG_ERROR, 
    		  "bme680_init failed with status %i", 
    		  rslt );
    
}

void bme_initialize ( void )
{
	uint8_t set_required_settings, rslt;
    uint16_t meas_period;
	lib_init();

    /* Set the temperature, pressure and humidity settings */
    gas_sensor.tph_sett.os_hum = BME680_OS_2X;
    gas_sensor.tph_sett.os_pres = BME680_OS_4X;
    gas_sensor.tph_sett.os_temp = BME680_OS_8X;
    gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
    gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    gas_sensor.power_mode = BME680_FORCED_MODE; 

    /* Set the required sensor settings needed */
    set_required_settings = 
    	BME680_OST_SEL | 
    	BME680_OSP_SEL | 
    	BME680_OSH_SEL | 
    	BME680_FILTER_SEL /*| 
    	BME680_GAS_SENSOR_SEL*/;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings,&gas_sensor);
    	
    cserror( rslt == BME680_OK, 
    		  LOG_ERROR, 
    		  "bme680_set_sensor_settings failed with status %i", 
    		  rslt );

    /* Set the power mode */
    rslt = bme680_set_sensor_mode(&gas_sensor);
    
    cserror( rslt == BME680_OK, 
    		  LOG_ERROR, 
    		  "bme680_set_sensor_mode failed with status %i", 
    		  rslt );
    		  
    bme680_get_profile_dur(&meas_period, &gas_sensor);
    
    bme_loop_delay = meas_period;
}

int bme_process ( void )
{
	int8_t rslt;
	rslt = bme680_get_sensor_data(&data, &gas_sensor);
	
    if ( rslt == BME680_W_NO_NEW_DATA )
    	return 0;
    
    cserror( rslt == BME680_OK, 
    		  LOG_ERROR, 
    		  "bme680_get_sensor_data failed with status %i", 
    		  rslt );
    		  
    bme_temperature = data.temperature / 100.;
    bme_pressure    = data.pressure    / 100.;
    bme_humidity    = data.humidity    / 1000.;
    
    /* Trigger the next measurement if you would like to read data out continuously */
    if (gas_sensor.power_mode == BME680_FORCED_MODE) {
    	rslt = bme680_set_sensor_mode(&gas_sensor);
    
	    cserror( rslt == BME680_OK, 
	    		  LOG_ERROR, 
	    		  "bme680_set_sensor_mode failed with status %i", 
	    		  rslt );
    }

	return 1;
}
