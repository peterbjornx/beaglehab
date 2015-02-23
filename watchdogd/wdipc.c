/* 
 * This file is part of watchdogd.
 *
 * watchdogd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * watchdogd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with watchdogd.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	23/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

/*
 * We need the process manager header
 */
#include "procmgr.h"

/*
 * We need hab/llist for the process list
 */
#include <hab/llist.h>

/*
 * We need hab/cslog for logging
 */
#include <hab/cslog.h>

/*
 * We need hab/cswdog for the watchdog IPC interface
 */
#include <hab/cswdog.h>

/*
 * We need sys/types for the pid type
 */
#include <sys/types.h>

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
 * We need time_t for the watchdog
 */
#include <time.h>

/*
 * We need sys/ipc for the SysV IPC API 
 */
#include <sys/ipc.h>

/*
 * We need sys/msg for the SysV message queue API 
 */
#include <sys/msg.h>

int wdipc_mqueue_handle;

void wdipc_initialize ( void )
{
	key_t key;

	/* Generate the IPC key */
	key = ftok( CS_WATCHDOG_TOKEN_FILE, 0 );

	/* Check for errors */
	cserror( key != -1, 
		LOG_ERROR, 
		"Could not open watchdog IPC channel: %i(%s)",
		errno,
		strerror ( errno ) );

	/* Open the message queue */
	wdipc_mqueue_handle = msgget ( key, IPC_CREAT | CS_WATCHDOG_IPC_MODE );

	/* Check for errors */
	cserror( wdipc_mqueue_handle != -1, 
		LOG_ERROR, 
		"Could not open watchdog IPC channel: %i(%s)",
		errno,
		strerror ( errno ) );
	
}

void wdipc_process ( void )
{
	int 		 ctr;
	cswdog_reset_t	 buffer;
	ssize_t 	 msgsz;
	time_t		 now;
	wd_proc_t	*process;

	/* Acquire the current time */
	time ( &now );

	/* Loop while less than 32 messages are received */
	for ( ctr = 0; ctr < 32; ctr++ ) {

		/* Receive reset requests */
		msgsz = msgrcv ( wdipc_mqueue_handle, 
				 &buffer, 
				 sizeof ( cswdog_reset_t ), 
				 CS_WATCHDOG_MTYPE,
				 IPC_NOWAIT );

		/* Check for errors */		
		if ( msgsz == -1 ) {
		
			/* An error has occurred */
			if ( errno == EINTR ) {

				/* System call was interrupted, retry */
				continue;

			} else if ( ( errno == EAGAIN ) ||
				    ( errno == ENOMSG ) ) {

				/* No message was received, return */
				return;

			} else if ( errno == E2BIG ) {
			
				/* Invalid size */ 

				/* Log event */
				cs_log( LOG_WARN, 
				"Invalid size watchdog reset received: %i", 
					msgsz );

				/* Continue */
				continue;

			} else {

				/* An error occurred */
				cs_log_fatal( LOG_ERROR, 
				"Could not read watchdog IPC channel: %i(%s)",
				errno,
				strerror ( errno ) );

			}			
		
		} else if ( /* Verify message size */
			 msgsz != ( sizeof ( cswdog_reset_t ) - sizeof( long ) ) 
			  ) {
			
			/* Invalid size */ 
			

			/* Log event */
			cs_log( LOG_WARN, 
				"Invalid size watchdog reset received: %i", 
				msgsz );

			/* Continue */
			continue;

		}
		
		/* Message size is valid, verify pid */
		if ( buffer.pid <= 0 ) {

			/* Invalid pid */			

			/* Log event */
			cs_log( LOG_WARN, 
				"Invalid watchdog reset pid received: %i", 
				buffer.pid );

			/* Continue */
			continue;
		}

		/* Message is valid */

		/* Get process to reset timer on */
		process = pm_get_process ( buffer.pid );

		/* Check if process exists */
		if ( process == NULL ) {

			/* Process does not exist */			

			/* Log event */
			cs_log( LOG_WARN, 
				"Tried to reset unknown process %i", 
				buffer.pid );

			/* Continue */
			continue;
		}

		/* Process exists */
		process->last_reset = now;

	}
}
