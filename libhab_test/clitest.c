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
#include <hab/cswdog.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

cs_chan_t *ipc_chan;
cstest_ipc_t buffer;

int main( int argc, char **argv ) 
{
	time_t curtime;
	double latency;
	int delay;
	csipc_set_program( argv[4] );
	cswdog_initialize ( ) ;
	assert(argc >= 5);
	ipc_chan = csipc_open_channel( argv[1], sizeof(cstest_ipc_t), atoi(argv[2]));
	delay = atoi(argv[3]);
	for (;;) {
		csipc_client_process(ipc_chan);
		memcpy ( &buffer, ipc_chan->pl_buffer, sizeof (cstest_ipc_t));
		curtime = time(NULL);
		latency = difftime(curtime, buffer.timestamp);
		printf("latency: %.f serial:%i\n", latency, buffer.serial);
		cswdog_reset_watchdog ( );
		sleep(delay);
	}
}
