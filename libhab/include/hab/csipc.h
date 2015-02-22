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
 * Created:	18/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __csipc_h__
#define __csipc_h__

/**
 * We need stdint for the unsigned integer type definitions.
 */
#include <stdint.h>

/*
 * Include llist for linked list functionality
 */
#include <hab/llist.h>

/*
 * Include mqueue for POSIX message queues
 */
#include <mqueue.h>

/*
 * Include time for timeouts
 */
#include <time.h>

/**
 * IPC Options
 */

#define CSIPC_MODE		(0666)
#define CSIPC_ANNOUNCE_SIZE	(256)
#define CSIPC_ANNOUNCE_MAXMSG	(10)
#define CSIPC_UPDATE_TIMEOUT	(2)
#define CSIPC_ANNOUNCE_TIMEOUT	(30)

/**
 * IPC channel definitions
 */

#define CS_SES_ANNOUNCE	("announce")
#define CS_CHAN_GPS	("gps")
#define CS_CHAN_ACC	("accelerometer")
#define CS_CHAN_GYRO	("gyroscope")
#define CS_CHAN_MAG	("magnetometer")
#define CS_CHAN_IMU	("interial")
#define CS_CHAN_AP	("autopilot")
#define CS_CHAN_BAR	("barometer")
#define CS_CHAN_HUM	("humidity")

/**
 * Structure describing an IPC channel
 */
typedef struct cs_chan cs_chan_t;

struct cs_chan {
	llist_t 	ll_link;
	char *		ls_name;
	mqd_t		mq_handle;
	int		mq_size;
	size_t		pl_size;
	void *		pl_buffer;
	time_t		ch_lastupd;
	cs_chan_t *	ch_announce;
};

/**
 * Structure describing an IPC server
 */

typedef struct {
	char *		ch_name;
	cs_chan_t *	ch_announce;
	llist_t		ch_listeners;
	int		mq_size;
	size_t		pl_size;
	void *		pl_buffer;
} cs_srv_t;

void csipc_set_program( const char *name );

cs_chan_t *csipc_open_channel( const char *name, size_t msg_size, int max_msg );

cs_srv_t *csipc_create_server( const char *name, size_t msg_size, int max_msg );

void csipc_server_process( cs_srv_t *server );

void csipc_client_process( cs_chan_t *channel );

#endif



