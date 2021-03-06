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
 * We need unistd for the process management functions
 */
#include <unistd.h>

/*
 * We need signal for the kill function
 */
#include <signal.h>

llist_t pm_process_list;

llist_t pm_respawn_queue;

llist_t pm_kill_queue;

void pm_initialize( void )
{
	/* Create llist head nodes */
	llist_create( &pm_process_list );
	llist_create( &pm_respawn_queue );
	llist_create( &pm_kill_queue );
}

int pm_get_process_iterator( llist_t *node, void *param )
{
	wd_proc_t	*process = ( wd_proc_t * ) node;
	pid_t		*pid 	 = ( pid_t * ) param;
	return process->pid == *pid;
}

wd_proc_t *pm_get_process( pid_t pid )
{
	/* Select the process from the list */
	return ( wd_proc_t * ) llist_iterate_select ( &pm_process_list, 
						      &pm_get_process_iterator,
						      &pid );
}

int pm_spawn_process ( wd_proc_t *process )
{
	pid_t	pid;
	time_t	now;

	/* Check for NULL pointers */
	csassert( process != NULL );

	/* Log what we are doing */
	cs_log( LOG_INFO, "Spawning %s", process->name );

	/* Acquire the current time */
	time ( &now );

	/* Fork */
	pid = fork ( );

	/* Check whether fork failed */
	if ( pid == -1 ) {

		/* Log this event */
		cs_log(	LOG_ERROR,
			"Could not fork: %i(%s)", 
			errno, 
			strerror( errno ) );

		/* Return false to indicate failure */
		return 0;
	}

	/* Check whether we are parent or child */
	if ( pid == 0 ) {

		/* We are the child, proceed to execute process */	
		if ( execv ( process->path, process->args ) == -1 ) {
			
			/* Exec failed */

			/* Log this event */
			cs_log(	LOG_ERROR,
				"Could not execv: %i(%s)", 
				errno, 
				strerror( errno ) );
			
			/* Exit */
			_exit ( 127 );

			/* Never reached but we need to appease the compiler */
			return 0;
		}	
			
		/* Never reached but we need to appease the compiler */
		return 1;
		
	} else {
	
		/* We are the parent, save the pid */
		process->pid = pid;

		/* Prevent immediate watchdog-kill */
		process->last_reset = now;

		/* Fork was successful, if exec fails we will find out through*/
		/* waitpid */
		
		/* Return true to indicate success */
		return 1;
	}
}

int pm_kill_process ( wd_proc_t *process )
{

	/* Check for NULL pointers */
	csassert( process != NULL );

	/* Check whether process currenly runs to prevent killing the pgrp */
	if ( process->pid == 0 )
		/* Process was not running, return false to indicate failure */
		return 0;

	/* Send SIGKILL to the process */
	if ( kill ( process->pid, SIGKILL ) == -1 ) {
		
		/* Failed to send the signal */

		/* Handle errors */
		if ( errno == ESRCH ) {
			
			/* The process is already dead */
			cs_log (LOG_WARN, 
				"Failed to kill %s: process died", 
				process->name);

			/* We were not successful but the process is gone */
			/* anyway so return true to indicate success */
			return 1;
		} else {
			
			/* We could not kill the process and it still lives */
			cs_log (LOG_ERROR, 
				"Failed to kill %s: %i(%s)", 
				errno,
				strerror( errno ));
			
			/* Note: we are in SERIOUS trouble here: a process */
			/* froze but could not be killed and restarted! */		

			/* Return false to indicate failure */
			return 0;
		}

	}

	/* Return true to indicate success */
	return 1;

}

void pm_request_respawn ( wd_proc_t *process )
{
	wd_respawn_t	*request;
	
	/* Check for NULL pointers */
	csassert( process != NULL );

	/* Allocate memory for the request */
	request = malloc ( sizeof ( wd_respawn_t ) );

	/* Handle out of memory error */
	cserror( request != NULL, 
		LOG_ERROR, 
		"Could not allocate process respawn request");
	
	/* Fill its fields */
	request->process = process;
	
	/* Append it to the queue */
	llist_add_end ( &pm_respawn_queue, ( llist_t * ) request );

} 

void pm_request_kill ( wd_proc_t *process, int action )
{
	wd_kill_t	*request;
	
	/* Check for NULL pointers */
	csassert( process != NULL );

	/* Allocate memory for the request */
	request = malloc ( sizeof ( wd_kill_t ) );

	/* Handle out of memory error */
	cserror( request != NULL, 
		LOG_ERROR,
		"Could not allocate process kill request");
	
	/* Fill its fields */
	request->process = process;
	request->action  = action;
	
	/* Append it to the queue */
	llist_add_end ( &pm_kill_queue, ( llist_t * ) request );

} 

void pm_add_process ( 	const char *name, 
			int wd_timeout, 
			const char *path, 
			const char **args )
{
	int		 ctr;
	int		 argc;
	wd_proc_t	*process;

	/* Check for NULL pointers */
	csassert( name != NULL );
	csassert( path != NULL );
	csassert( args != NULL );

	/* Allocate new process structure */
	process = malloc ( sizeof ( wd_proc_t ) );

	/* Handle out of memory error */
	cserror( process != NULL, 
		LOG_ERROR, 
		"Could not allocate process object");

	/* Allocate process name */
	process->name = malloc ( strlen ( name ) + 1 );

	/* Handle out of memory error */
	cserror( process->name != NULL, 
		LOG_ERROR, 
		"Could not allocate process name");

	/* Copy process name */
	strcpy ( process->name, name );

	/* Allocate process path */
	process->path = malloc ( strlen ( path ) + 1 );

	/* Handle out of memory error */
	cserror( process->name != NULL, 
		LOG_ERROR, 
		"Could not allocate process path");

	/* Copy process path */
	strcpy ( process->path, path );

	/* Count process arguments */
	argc = -1;
	while ( args[ ++argc ] != NULL );

	/* Allocate process arguments */
	process->args = malloc ( ( argc + 1 ) * sizeof ( char * ) );

	/* Handle out of memory error */
	cserror( process->args != NULL, 
		LOG_ERROR, 
		"Could not allocate process arguments");

	/* Allocate and copy arguments */
	for ( ctr = 0; ctr < argc; ctr++ ) {

		/* Allocate argument */
		process->args[ ctr ] = malloc ( strlen( args[ ctr ] ) + 1 );

		/* Handle out of memory error */
		cserror( process->args[ ctr ] != NULL, 
			LOG_ERROR, 
			"Could not allocate process argument");

		/* Copy the argument */
		strcpy ( process->args[ ctr ], args[ ctr ] );

	}

	/* Set argument list terminator */
	process->args[ ctr ] = NULL;
	
	/* Fill process fields */
	process->wd_timeout	= wd_timeout;
	process->pid		= 0;
	process->last_reset	= 0;
	process->freeze_count	= 0;
	process->term_count	= 0;
	process->last_status	= 0;

	/* Add process to list */
	llist_add_end ( &pm_process_list, ( llist_t * ) process );

	/* Request process start */
	pm_request_respawn ( process );

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

					/* Ignore hard-killed unknown children*/
					/* A watchdog-kill-respawn will erase */
					/* the old pid so this will yield a */
					/* false positive here otherwise */					
					if ( ( !WIFSIGNALED( w_status ) ) ||
					     (WTERMSIG(w_status) != SIGKILL) ) {
			
						/* We did not, log this event */
						cs_log(	LOG_WARN, 
							"Unknown child died: %i",
							(int) w_pid );
					}
					
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
			pm_request_kill ( process, WD_RESPAWN );
									
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
			request->process->name );
		
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
				request->process->name );
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
			request->process->name );
		
		/* Try to kill it */
		if ( pm_kill_process( request->process ) ) {
		
			/* Kill was successful */

			/* Do we need to respawn this process? */
			if ( request->action == WD_RESPAWN ) {
	
				/* Request respawn */
				pm_request_respawn ( request->process );
			
			}

			/* Remove this request from the queue */
			llist_unlink ( ( llist_t * ) request );

			/* Free the request */
			free ( request );

		} else {

			/* Kill failed */
									
			/* Log this event */
			cs_log( LOG_INFO, 
				"Could not kill child %s",
				request->process->name );
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
