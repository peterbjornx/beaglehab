/* 
	This file is part of radiod.

	radiod is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	radiod is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with radiod.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "radiod.h"
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>

#define MAX_RETRIES 10

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};
/*
** initsem() -- more-than-inspired by W. Richard Stevens' UNIX Network
** Programming 2nd edition, volume 2, lockvsem.c, page 295.
*/
int initsem(key_t key, int nsems)  /* key from ftok() */
{
    int i;
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;
    int semid;

    semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);

    if (semid >= 0) { /* we got it first */
        sb.sem_op = 1; sb.sem_flg = 0;
        arg.val = 1;


        for(sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++) { 
            /* do a semop() to "free" the semaphores. */
            /* this sets the sem_otime field, as needed below. */
            if (semop(semid, &sb, 1) == -1) {
                int e = errno;
                semctl(semid, 0, IPC_RMID); /* clean up */
                errno = e;
                return -1; /* error, check errno */
            }
        }

    } else if (errno == EEXIST) { /* someone else got it first */
        int ready = 0;

        semid = semget(key, nsems, 0); /* get the id */
        if (semid < 0) return semid; /* error, check errno */

        /* wait for other process to initialize the semaphore: */
        arg.buf = &buf;
        for(i = 0; i < MAX_RETRIES && !ready; i++) {
            semctl(semid, nsems-1, IPC_STAT, arg);
            if (arg.buf->sem_otime != 0) {
                ready = 1;
            } else {
                sleep(1);
            }
        }
        if (!ready) {
            errno = ETIME;
            return -1;
        }
    } else {
        return semid; /* error, check errno */
    }

    return semid;
}

int gps_sem;
int gps_shm;
csproto_gps_t *gpsblock;



void gps_shm_open()
{
	key_t key;
	/* make the key: */
	if ((key = ftok("/cs_tok", 'G')) == -1) {
		p_log(LOG_ERROR, "Could not get SHM key: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	 /* connect to (and possibly create) the segment: */
	if ((gps_shm = shmget(key, sizeof(csproto_gps_t), 0644 | IPC_CREAT)) == -1) {
		p_log(LOG_ERROR, "Could not get SHM segment: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if ((key = ftok("/cs_tok", 'g')) == -1) {
		p_log(LOG_ERROR, "Could not get semaphore key: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	gpsblock = shmat(gps_shm, (void *)0, 0);
	if (gpsblock == (csproto_gps_t *)(-1)) {
		p_log(LOG_ERROR, "Could not attach SHM segment: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	gps_sem = initsem(key, 1);
	if (gps_sem < 0) {
		p_log(LOG_ERROR, "Could not create semaphore: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);		
	}	
	
}
void gps_sem_lock(){
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;  /* set to allocate resource */
	sb.sem_flg = SEM_UNDO;
	if (semop(gps_sem, &sb, 1) == -1) {
		p_log(LOG_ERROR, "Could not lock semaphore: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);		
	}
}
void gps_sem_unlock(){
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = 1;  /* set to allocate resource */
	sb.sem_flg = SEM_UNDO;
	if (semop(gps_sem, &sb, 1) == -1) {
		p_log(LOG_ERROR, "Could not unlock semaphore: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);		
	}
}


csproto_packet_t 	pakketje;
csproto_telemetry_t	telemetry;

int main( int argc, char **argv ) 
{
	/* Not really a daemon... */
	int id = 0;
	FILE *radio = radio_open("/dev/ttyO4");
	gps_shm_open();
	telemetry.lat_degrees  = 52.0;
	telemetry.lat_minutes  = 16.0 + 24.523/60.0;
	telemetry.long_degrees =  6.0;
	telemetry.long_minutes = 47.0 +  52.161/60.0;
	telemetry.altitude     = 1.50;
	telemetry.velocity     = 42;
	telemetry.heading      = 56.78;
	telemetry.acc_x	       = 999.567;
	telemetry.acc_y	       = -999.566;
	telemetry.acc_z	       = 0;
	for (;;) {
		gps_sem_lock();
		/*telemetry.lat_degrees = gpsblock->lat_degrees;
		telemetry.lat_minutes = gpsblock->lat_minutes;
		telemetry.long_degrees = gpsblock->long_degrees;
		telemetry.long_minutes = gpsblock->long_minutes;
		telemetry.velocity = gpsblock->velocity;
		telemetry.heading = gpsblock->heading;
		telemetry.altitude = gpsblock->altitude;
		*/gps_sem_unlock();
		csproto_encode_telemetry(&pakketje, &telemetry);
		csproto_prepare(&pakketje);
		radio_send(radio, &pakketje);
		pakketje.payload_id = id;
		p_log(LOG_TRACE, "Transmitting packet with telemetry data: LN:%f LT:%f\n", telemetry.lat_degrees, telemetry.long_degrees);
		id++;
		id &= 3;
	}
}
