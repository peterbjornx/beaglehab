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
 * Created:	22/02/2015
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
 * We need sys/types for the pid type
 */
#include <sys/types.h>

/*
 * We need sys/wait for the process wait API
 */
#include <sys/wait.h>

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

llist_t pm_process_list;

llist_t pm_respawn_queue;

llist_t pm_kill_queue;

int pm_get_process_iterator( llist_t *node, void *param )
{
	wd_proc_t	*process = ( wd_proc_t * ) node;
	pid_t		*pid 	 = ( pid_t * ) param;
	return process->pid == pid;
}

wd_proc_t *pm_get_process( pid_t pid )
{
	/* Select the process from the list */
	return ( wd_proc_t * ) llist_iterate_select ( &pm_process_list, 
						      &pm_get_process_iterator,
						      &pid );
}

void pm_request_respawn ( wd_proc_t *process )
{
	wd_respawn_t	*request;
	
	/* Check for NULL pointers */
	csassert( process != NULL );

	/* Allocate memory for the request */
	request = malloc ( sizeof ( wd_respawn_t ) );

	/* Handle out of memory error */
	cserror( request != NULL, "Could not allocate process respawn request");
	
	/* Fill its fields */
	request->process = process;
	
	/* Append it to the queue */
	llist_add_end ( &pm_respawn_queue, ( llist_t * ) request );

} 

void pm_request_kill ( wd_proc_t *process )
{
	wd_kill_t	*request;
	
	/* Check for NULL pointers */
	csassert( process != NULL );

	/* Allocate memory for the request */
	request = malloc ( sizeof ( wd_respawn_t ) );

	/* Handle out of memory error */
	cserror( request != NULL, "Could not allocate process kill request");
	
	/* Fill its fields */
	request->process = process;
	
	/* Append it to the queue */
	llist_add_end ( &pm_kill_queue, ( llist_t * ) request );

} 
 
void pm_collect_children ( void )
{
	pid_t		 w_pid;
	int		 w_status;
	wd_proc_t	*process;
	
	/* Loop until all children whose status has changed are collected */
	do {
		w_pid = waitpid ( -1, &w_status, WNOHANG );
		
		if ( w_pid == -1 ) {
			/* An error occurred during waitpid, handle it */

			if ( errno == ECHILD ) {

				/* No children remain, ignore and return */

 			} else if ( errno == EINTR ) {

				/* System call was interrupted by a signal, */
				/* ignore this error and try again */
				
				/* We set w_pid to 1 to force the loop to run */
				/* once more */
				w_pid = 1; 

			} else {

				/* An error occurred, report this and die */
				cs_log_fatal(LOG_ERROR, 
			"An error occurred while collecting children: %i(%s)", 
						errno,
						strerror(errno));

			}		
		} else if ( w_pid > 0 ) {
						
			/* The status of a child has changed */

			if ( WIFEXITED( w_status ) || WIFSIGNALED( w_status ) ){
				
				/* The child has terminated */
			
				/* Look it up in the table */
				process = pm_get_process ( w_pid );

				/* Check if we found it */
				if ( process == NULL ) {
					
					/* We did not, log this event */
					cs_log(	LOG_WARN, 
						"Unknown child died: %i",
						(int) w_pid );
					
					/* Continue on to next child */
					continue;
				}

				/* Set the process's pid to zero to indicate */
				/* its death */
				process->pid = 0;

				/* Store the status */
				process->last_status = w_status;

				/* Bump the term counter */
				process->term_count++;
	
				/* Request its respawn */
				pm_request_respawn ( process );
									
				/* Log this event */
				cs_log( LOG_INFO,
					"Child %s terminated",
					process->name );		
						
			} 

		}
	
	} while ( w_pid > 0 );

}

void pm_process_watchdog ( void )
{
	llist_t		*node;
	wd_proc_t	*process;
	time_t		 now;
	double		 elapsed;
	
	/* Acquire current time */
	time ( &now );

	/* Iterate over the process list */
	for (	node = pm_process_list.next ; 
		node != &( pm_process_list ) ;
		node = node->next ) {
		
		process = (wd_proc_t *) node;

		/* Skip dead processes */
		if ( process->pid == 0 )
			continue;

		/* Determine how many seconds have passed since last wd reset */
		elapsed = difftime ( now, process->last_reset );

		if ( elapsed > process->wd_timeout ) {

			/* Watchdog timer for process has expired */

			/* Bump freeze counter */
			process->freeze_count++;

			/* Add process to kill queue */			
			pm_request_kill ( process );
									
			/* Log this event */
			cs_log( LOG_INFO, 
				"Watchdog timer elapsed for child %s",
				process->name );

		}
	
	}

}

void pm_process_respawns ( void )
{
	llist_t		*node;
	wd_respawn_t	*request;
	
	/* Iterate over the respawn queue */
	for (	node = pm_respawn_queue.next ; 
		node != &( pm_respawn_queue ) ; ) {
		
		request = (wd_respawn_t *) node;

		/* Select next request */
		node = node->next;	
	
		/* Log this event */
		cs_log( LOG_INFO, 
			"Respawning child %s",
			process->name );
		
		/* Try to respawn it */
		if ( pm_spawn_process( request->process ) ) {
		
			/* Respawn was successful */

			/* Remove this request from the queue */
			llist_unlink ( ( llist_t * ) request );

			/* Free the request */
			free ( request );

		} else {

			/* Respawn failed */
									
			/* Log this event */
			cs_log( LOG_INFO, 
				"Could not respawn child %s",
				process->name );
		}

	}
}

void pm_process_kills ( void )
{
	llist_t		*node;
	wd_kill_t	*request;
	
	/* Iterate over the kill queue */
	for (	node = pm_kill_queue.next ; 
		node != &( pm_kill_queue ) ; ) {
		
		request = (wd_kill_t *) node;

		/* Select next request */
		node = node->next;	
	
		/* Log this event */
		cs_log( LOG_INFO, 
			"Killing frozen child %s",
			process->name );
		
		/* Try to respawn it */
		if ( pm_kill_process( request->process ) ) {
		
			/* Respawn was successful */

			/* Remove this request from the queue */
			llist_unlink ( ( llist_t * ) request );

			/* Free the request */
			free ( request );

		} else {

			/* Respawn failed */
									
			/* Log this event */
			cs_log( LOG_INFO, 
				"Could not kill child %s",
				process->name );
		}

	}
}

void pm_process ( void )
{
	pm_collect_children();
	pm_process_watchdog();
	pm_process_kills();
	pm_process_respawns();
}
