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
 * Created:	22/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "testipc.h"
#include <hab/csipc.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

cs_srv_t *ipc_chan;
cstest_ipc_t buffer;

int main( int argc, char **argv ) 
{
	time_t curtime;
	double latency;
	int delay;
	csipc_set_program( "srvtest" );
	assert(argc >= 4);
	ipc_chan = csipc_create_server( argv[1], sizeof(cstest_ipc_t), atoi(argv[2]));
	delay = atoi(argv[3]);
	for (;;) {
		curtime = time(NULL);
		buffer.timestamp = curtime;
		buffer.serial++;
		memcpy ( ipc_chan->pl_buffer, &buffer, sizeof (cstest_ipc_t));
		csipc_server_process(ipc_chan);
		sleep(delay);
	}
}
