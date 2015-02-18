/* 
	This file is part of gpsd.

	gpsd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gpsd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gpsd.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __gpsd_h__
#define __gpsd_h__

#define LOG_TRACE	(0)
#define LOG_DEBUG	(1)
#define LOG_INFO	(2)
#define LOG_WARN	(3)
#define LOG_ERROR	(4)
#define LOG_MAX		(LOG_ERROR)

void p_log ( int level, const char *format, ...);

FILE *gps_open(const char *path);

size_t	gps_read_sentence(FILE *gps, char *buffer, size_t buffer_size);

#endif
