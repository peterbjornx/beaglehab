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

#ifndef __csi2c_h__
#define __csi2c_h__

/*
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

/*
 * We need stddef for the size_t type 
 */
#include <stddef.h>

/*
 * CSI2C Options
 */

#define CSI2C_BUS_DEVICE_FORMAT		("/dev/i2c-%i")

typedef struct {
	int	dev_fd;
} csi2c_bus_t;

csi2c_bus_t *csi2c_open_bus ( int id ); 

int	csi2c_write_register ( 	csi2c_bus_t *bus,
				long 	 slave_addr, 
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size );

int	csi2c_read_register ( 	csi2c_bus_t *bus,
				long 	 slave_addr, 
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size );

int	csi2c_read_device( 	csi2c_bus_t *bus,
				long 	 slave_addr, 
				void 	*buffer,
				size_t	 size );

#endif



