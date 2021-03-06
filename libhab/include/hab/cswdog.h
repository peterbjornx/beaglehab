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

#ifndef __cswdog_h__
#define __cswdog_h__

/**
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

/*
 * Include time for timeouts
 */
#include <time.h>

/**
 * Watchdog Options
 */

#define CS_WATCHDOG_TOKEN_FILE	("/var/run/cswdog")
#define CS_WATCHDOG_IPC_MODE	(0666)
#define CS_WATCHDOG_MTYPE	(1)

/**
 * Watchdog reset message structure
 */
typedef struct {
	long	mtype;
	pid_t	pid;
} cswdog_reset_t;

void cswdog_initialize ( void );

void cswdog_reset_watchdog ( void );

#endif



