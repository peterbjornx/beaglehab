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
 * Include libhab I2C API defs
 */
#include <hab/csi2c.h>

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
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

csi2c_bus_t *csi2c_open_bus ( int id )
{
	char		 filename[80];
	csi2c_bus_t 	*bus;	

	/* Try to allocate the bus object */
	bus = malloc ( sizeof( csi2c_bus_t ) );
	
	/* Check for errors */
	cserror( bus != NULL, 
		 LOG_ERROR, 
		 "Ran out of memory allocating I2C bus" );
	
	/* Generate device filename */
	sprintf ( filename, CSI2C_BUS_DEVICE_FORMAT, id );

	/* Try to open the bus */
	bus->dev_fd = open ( filename, O_RDWR );

	/* Check for errors */
	cserror( bus->dev_fd >= 0, 
		 LOG_ERROR,
		 "Could not open I2C bus (%s): %i(%s)",
		 filename,
		 errno,
		 strerror ( errno ) );

	/* Return the bus */
	return bus;

}

int	csi2c_write_register( 	csi2c_bus_t *bus,
				long 	 slave_addr, 
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size )
{
	struct i2c_msg			tr_queue[2];
	struct i2c_rdwr_ioctl_data	tr_desc;

	uint8_t	obuf[33];

	/* Check for NULL pointers */
	csassert( bus != NULL );
	csassert( ( size == 0 ) || ( buffer != NULL ) );
	csassert( size <= 32 );

	/* Setup transaction descriptor */
	tr_desc.msgs	  = tr_queue;
	tr_desc.nmsgs	  = 1;	

	/* Setup message descriptor for the packet */
	tr_queue[0].addr  = ( __u16 ) slave_addr;
	tr_queue[0].flags = ( __u16 ) 0;
	tr_queue[0].len	  = ( __u16 ) size + 1; 
	tr_queue[0].buf	  = ( __u8 *) obuf;
	
	/* Add the register address to the output buffer */
	obuf[0] = reg_addr;

	/* If necessary, copy payload to packet */
	if ( size != 0 ) {

		memcpy(&(obuf[1]), buffer, size);

	}

	/* Execute the transaction */
	return ioctl ( bus->dev_fd, I2C_RDWR, &tr_desc );		

}

int	csi2c_read_register( 	csi2c_bus_t *bus,
				long 	 slave_addr, 
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size )
{
	struct i2c_msg			tr_queue[2];
	struct i2c_rdwr_ioctl_data	tr_desc;

	/* Check for NULL pointers */
	csassert( bus != NULL );
	csassert( ( size == 0 ) || ( buffer != NULL ) );

	/* Setup transaction descriptor */
	tr_desc.msgs	  = tr_queue;
	tr_desc.nmsgs	  = 1;	

	/* Setup message descriptor for the register address selection packet */
	tr_queue[0].addr  = ( __u16 ) slave_addr;
	tr_queue[0].flags = ( __u16 ) 0;
	tr_queue[0].len	  = ( __u16 ) 1; 
	tr_queue[0].buf	  = ( __u8 *) &reg_addr;
	
	/* If necessary, setup message descriptor for the data packet */
	if ( size != 0 ) {

		/* Modify transaction descriptor to include the second msg*/
		tr_desc.nmsgs = 2;

		/* Setup message descriptor for the data packet */
		tr_queue[1].addr  = ( __u16 ) slave_addr;
		tr_queue[1].flags = ( __u16 ) I2C_M_RD;
		tr_queue[1].len	  = ( __u16 ) size; 
		tr_queue[1].buf	  = ( __u8 *) buffer;

	}

	/* Execute the transaction */
	return ioctl ( bus->dev_fd, I2C_RDWR, &tr_desc );		

}



