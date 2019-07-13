/* 
 * This file is part of libhab.
 *
 * libhab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * libhab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with libhab.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	23/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/**
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

/*
 * We need stdlib for the memory management functions
 */
#include <stdlib.h>

/*
 * Include string for str‌ing and memory manipulation
 */
#include <string.h>

/*
 * Include logging functionality
 */
#include <hab/cslog.h>

/*
 * Include libhab SPI API defs
 */
#include <hab/csspi.h>

/*
 * We need stdio for sprintf
 */
#include <stdio.h>

/*
 * Include errno.h for error handling
 */
#include <errno.h>

/*
 * We need unistd for the POSIX file API
 */
#include <unistd.h>

/*
 * We need fcntl, sys/types and sys/stat for the POSIX open call
 */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * We need ioctl to call the I2C bus driver
 */
#include <sys/ioctl.h>

/*
 * We also need the linux I2C API
 */
#include <linux/spi/spidev.h>
#include <linux/types.h>

csspi_dev_t *csspi_open_dev ( int id, int cs, int mode, int lsb )
{
	int			    s;
	char		 filename[80];
	csspi_dev_t 	*bus;	

	/* Try to allocate the bus object */
	bus = malloc ( sizeof( csspi_dev_t ) );
	
	/* Check for errors */
	cserror( bus != NULL, 
		 LOG_ERROR, 
		 "Ran out of memory allocating SPI bus" );
	
	/* Generate device filename */
	sprintf ( filename, CSSPI_BUS_DEVICE_FORMAT, id, cs );

	/* Try to open the bus */
	bus->dev_fd = open ( filename, O_RDWR );

	/* Check for errors */
	cserror( bus->dev_fd >= 0, 
		 LOG_ERROR,
		 "Could not open SPI bus (%s): %i(%s)",
		 filename,
		 errno,
		 strerror ( errno ) );

	/* Do mode setting */
	s = ioctl( bus->dev_fd, SPI_IOC_WR_MODE32, &mode );
	
	cserror( s >= 0, 
		 LOG_ERROR,
		 "Could not set SPI mode (%s): %i(%s)",
		 filename,
		 errno,
		 strerror ( errno ) );

	/* Do LSB setting */
	s = ioctl( bus->dev_fd, SPI_IOC_WR_LSB_FIRST, &lsb );
	
	cserror( s >= 0, 
		 LOG_ERROR,
		 "Could not set SPI endianness (%s): %i(%s)",
		 filename,
		 errno,
		 strerror ( errno ) );

	/* Return the bus */
	return bus;

}

int	csspi_write_register( 	csspi_dev_t *bus,
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size )
{
	struct spi_ioc_transfer		tr_queue[1];

	uint8_t	obuf[65];
	uint8_t ibuf[65];

	/* Clear SPI transfer queue */
	memset( tr_queue, 0, sizeof tr_queue  );

	/* Check for NULL pointers */
	csassert( bus != NULL );
	csassert( ( size == 0 ) || ( buffer != NULL ) );
	csassert( size <= 64 );

	/* Setup message descriptor for the packet */
	tr_queue[0].len		= size + 1; 
	tr_queue[0].tx_buf  = ( uintptr_t ) obuf;
	tr_queue[0].rx_buf  = ( uintptr_t ) ibuf;
	
	/* Add the register address to the output buffer */
	obuf[0] = reg_addr;

	/* If necessary, copy payload to packet */
	if ( size != 0 ) {

		memcpy(&(obuf[1]), buffer, size);

	}

	/* Execute the transaction */
	return ioctl ( bus->dev_fd, SPI_IOC_MESSAGE(1), tr_queue );		

}

int	csspi_read_register( 	csspi_dev_t *bus,
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size )
{
	uint8_t dbuf[65];
	struct spi_ioc_transfer		tr_queue[2];

	/* Clear SPI transfer queue */
	memset( tr_queue, 0, sizeof tr_queue  );

	/* Check for NULL pointers */
	csassert( bus != NULL );
	csassert( ( size == 0 ) || ( buffer != NULL ) );
	csassert( size <= 64 );

	/* Setup message descriptor for the register address selection packet */
	tr_queue[0].len	   = 1; 
	tr_queue[0].tx_buf = ( uintptr_t ) &reg_addr;
	tr_queue[0].rx_buf = ( uintptr_t ) dbuf;
	
	/* If necessary, setup message descriptor for the data packet */
	if ( size != 0 ) {

		/* Setup message descriptor for the data packet */
		tr_queue[1].len	   = size; 
		tr_queue[1].rx_buf = ( uintptr_t ) buffer;
		tr_queue[1].tx_buf = ( uintptr_t ) dbuf;

	}
	
	/* Execute the transaction */
	return ioctl ( bus->dev_fd, SPI_IOC_MESSAGE((size==0)?1:2), tr_queue );

}