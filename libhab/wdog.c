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

/*
 * We need unistd for getpid
 */
#include <unistd.h>

int	cswdog_mqueue_handle;
pid_t	cswdog_local_pid;

void cswdog_initialize ( void )
{
	key_t key;

	/* Generate the IPC key */
	key = ftok( CS_WATCHDOG_TOKEN_FILE, 0 );

	/* Check for errors */
	cserror( key == -1, 
		LOG_ERROR, 
		"Could not open watchdog IPC channel: %i(%s)",
		errno,
		strerror ( errno ) );

	/* Open the message queue */
	cswdog_mqueue_handle = msgget ( key, IPC_CREAT | CS_WATCHDOG_IPC_MODE );

	/* Check for errors */
	cserror( cswdog_mqueue_handle == -1, 
		LOG_ERROR, 
		"Could not open watchdog IPC channel: %i(%s)",
		errno,
		strerror ( errno ) );

	/* Get our pid */
	cswdog_local_pid = getpid ( );
	
}

void cswdog_reset_watchdog ( void )
{
	int 		 ctr;
	cswdog_reset_t	 buffer;
	int		 status;

	/* Fill request fields */
	buffer.mtype = CS_WATCHDOG_MTYPE;
	buffer.pid   = cswdog_local_pid;

	/* Try to send reset request */
	for ( ctr = 0 ; ctr < 32 ; ctr++ ) {
		status = msgsnd ( cswdog_mqueue_handle, 
				  &buffer, 
				  sizeof ( cswdog_reset_t ), 
				  IPC_NOWAIT );

		/* Check for errors */		
		if ( status == -1 ) {
		
			/* An error has occurred */
			if ( errno == EINTR ) {

				/* System call was interrupted, retry */
				continue;

			} else if ( ( errno == EAGAIN ) ||
				    ( errno == ENOMEM ) ) {

				/* No message was received, return */
				continue;

			} else {

				/* An error occurred */
				cs_log_fatal( LOG_ERROR, 
				"Could not write watchdog IPC channel: %i(%s)",
				errno,
				strerror ( errno ) );

			}			
		
		}
		
		/* We were successful, return */
		return;
	}

	/* Failed to write 32 times */
	cs_log( LOG_WARN, 
		"Could not write watchdog IPC channel: %i(%s)",
		errno,
		strerror ( errno ) );
}
