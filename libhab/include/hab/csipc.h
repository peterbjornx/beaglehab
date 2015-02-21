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

typedef struct {
	llist_t 	ll_link;
	mqd_t		mq_handle;
	int		mq_size;
	size_t		pl_size;
	void *		pl_buffer;
} cs_chan_t;

/**
 * Structure describing an IPC listener
 */

typedef struct {
	char *		ch_name;
	cs_chan_t *	ch_announce;
	llist_t		ch_listeners;
	int		mq_size;
	size_t		pl_size;
	void *		pl_buffer;
} cs_srv_t;

typedef char[128] cs_announce_t;

cs_chan_t *csipc_open_channel(const char *name);

#endif



