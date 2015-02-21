/* 
 * This file is part of gpsd.
 *
 * gpsd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gpsd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gpsd.  If not, see <http://www.gnu.org/licenses/>.
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
#include "gpsd.h"

FILE *gps_open(const char *path)
{
	int fd;
	FILE *str;
	struct termios options;
	
	p_log(LOG_INFO, "Opening GPS TTY %s\n", path);

	fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd == -1) {
		p_log(LOG_ERROR, "Could not open TTY for GPS (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
		
	if (tcgetattr(fd, &options) == -1){
		p_log(LOG_ERROR, "Could not get attributes for GPS TTY (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);

	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_iflag &= ~(IXON | IXOFF | IXANY | ISTRIP | INLCR | ICRNL | IGNCR);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ICANON);
	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 0;
		
	if (tcsetattr(fd, TCSANOW, &options) == -1){
		p_log(LOG_ERROR, "Could not set attributes for GPS TTY (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	str = fdopen(fd, "rb");

	if (fcntl(fd, F_SETFL, 0) == -1){
		p_log(LOG_ERROR, "Could not set FD flags for GPS TTY (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (str == NULL) {
		p_log(LOG_ERROR, "Could not open GPS TTY as file stream (%s): %s!\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	p_log(LOG_INFO, "Opened GPS TTY\n");
	
	return str;
}

size_t	gps_read_sentence(FILE *gps, char *buffer, size_t buffer_size)
{
	char inbuf;
	int ptr = 0;
	int nl = 0;
	buffer_size--;
	for (ptr = 0; ptr < buffer_size; ptr++) {
		if (fread(&inbuf, 1, 1, gps) != 1) {
			p_log(LOG_ERROR, "Could not read from GPS TTY: %s!\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		buffer[ptr] = inbuf;
		if (nl && (inbuf == '\n')){
			buffer	[ptr + 1] = 0;
			return ptr + 1;
		} else if (inbuf == '\r')
			nl = 1;
		else
			nl = 0;
		
	}	
	buffer[buffer_size] = 0;
	return buffer_size;
}
