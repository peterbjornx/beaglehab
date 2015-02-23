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

#ifndef __procmgr_h__
#define __procmgr_h__

/*
 * We need llist for the process list
 */
#include <hab/llist.h>

/*
 * We need sys/types for the pid type
 */
#include <sys/types.h>

/*
 * We need time_t for the watchdog variables
 */
#include <time.h>


/* Definitions for the action parameter of pm_request_kill */
#define WD_RESPAWN	(1)
#define WD_KILL		(0)

typedef struct {

	/* Linked list node */
	llist_t		  node;

	/* Name of this process */
	char 		 *name;

	/* Spawn information */
	int		  wd_timeout;
	char		 *path;
	char		**args;
	
	/* Management information */
	pid_t		  pid;
	time_t		  last_reset;
	int		  freeze_count;
	int		  term_count;
	int		  last_status;
	
} wd_proc_t;

typedef struct {
	
	/* Linked list node */
	llist_t		 node;

	/* Process to respawn */
	wd_proc_t	*process;

} wd_respawn_t;

typedef struct {
	
	/* Linked list node */
	llist_t		 node;

	/* Process to kill */
	wd_proc_t	*process;

	/* Action to be taken after kill */
	int 		 action;

} wd_kill_t;

void pm_initialize( void );

void pm_add_process ( 	const char *name, 
			int wd_timeout, 
			const char *path, 
			const char **args );

wd_proc_t *pm_get_process( pid_t pid );

void pm_process ( void );

#endif
