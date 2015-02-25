/* 
 * This file is part of radiod.
 *
 * radiod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * radiod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with radiod.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "csproto.h"
#include <hab/cslog.h>

FILE *radio_open(const char *path)
{
	int fd;
	FILE *str;
	struct termios options;
	
	cs_log(LOG_INFO, "Opening RADIO TX TTY %s\n", path);

	fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		cs_log(LOG_ERROR, "Could not open TTY for RADIO TX (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
		
	if (tcgetattr(fd, &options) == -1){
		cs_log(LOG_ERROR, "Could not get attributes for RADIO TX TTY (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	cfsetispeed(&options, B300);
	cfsetospeed(&options, B300);

	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_iflag &= ~(IXON | IXOFF | IXANY);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;		
	if (tcsetattr(fd, TCSANOW, &options) == -1){
		cs_log(LOG_ERROR, "Could not set attributes for RADIO TX TTY (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	str = fdopen(fd, "w");

	if (fcntl(fd, F_SETFL, 0) == -1){
		cs_log(LOG_ERROR, "Could not set FD flags for RADIO TX TTY (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (str == NULL) {
		cs_log(LOG_ERROR, "Could not open RADIO TX TTY as file stream (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	cs_log(LOG_INFO, "Opened RADIO TX TTY\n");
	
	return str;

}

void radio_send(FILE *radio, csproto_packet_t *packet)
{
	size_t written = 0;

	csproto_prepare ( packet );

	written = fwrite(packet, sizeof(csproto_packet_t), 1, radio);

	if (written != 1) { 
		cs_log(LOG_ERROR, "Could not write to RADIO TX TTY: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);
	}



	if (fflush(radio) != 0) {
		cs_log(LOG_ERROR, "Could not wait on RADIO TX TTY: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (tcdrain(fileno(radio)) != 0) {
		cs_log(LOG_ERROR, "Could not wait on RADIO TX TTY: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void radio_printf(FILE *radio, const char *format, ...) 
{	
	va_list args;
	va_start ( args, format );
	if ( vfprintf(radio, format, args) < 0) {
		cs_log(LOG_ERROR, "Could not write to RADIO TX TTY: %s!\n", strerror(errno));
		exit(EXIT_FAILURE);
	};
	va_end ( args );
}
