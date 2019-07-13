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
 * Created:	13/07/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __csspi_h__
#define __csspi_h__

/*
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

/*
 * We need stddef for the size_t type 
 */
#include <stddef.h>

#include <linux/spi/spidev.h>

/*
 * CSSPI Options
 */

#define CSSPI_BUS_DEVICE_FORMAT		("/dev/spidev%i.%i")

#define CSSPI_MODE0                 (SPI_MODE_0)
#define CSSPI_MODE1                 (SPI_MODE_1)
#define CSSPI_MODE2                 (SPI_MODE_2)
#define CSSPI_MODE3                 (SPI_MODE_3)


typedef struct {
	int	dev_fd;
} csspi_dev_t;

csspi_dev_t *csspi_open_dev ( int id, int cs, int mode, int lsb ); 

int	csspi_read_register( 	csspi_dev_t *bus,
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size );

int	csspi_write_register( 	csspi_dev_t *bus,
				uint8_t  reg_addr, 
				void 	*buffer,
				size_t	 size );

#endif



