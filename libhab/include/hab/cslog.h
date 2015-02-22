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

#ifndef __cslog_h__
#define __cslog_h__

#define LOG_TRACE	(0)
#define LOG_DEBUG	(1)
#define LOG_INFO	(2)
#define LOG_WARN	(3)
#define LOG_ERROR	(4)
#define LOG_MAX		(LOG_ERROR)

/**
 * @brief Set the minimum log level
 * @param log_level The minimum log level displayed by the software
 */
void cs_set_log_level( int log_level );

/**
 * @brief Output a message to the log
 * @param level The level of the message
 * @param format The format string to output ( see *printf* )
 */
void cs_log ( int level, const char *format, ...);

/**
 * @brief Output a message to the log and die
 * @param level The level of the message
 * @param format The format string to output ( see *printf* )
 */
void cs_log_fatal ( int level, const char *format, ...);

#define csassert(CoNd) do { if ( !(CoNd) ) cs_log_fatal(LOG_ERROR, \
		"assertion %s failed at %s:%i", #CoNd , __FILE__, __LINE__); \
			} while (0)

#define cserror(CoNd, LeVeL, FoRmAt, ... ) do { if ( !(CoNd) ) \
						cs_log_fatal((LeVeL),(FoRmAt) \
							, ##__VA_ARGS__); \
					   } while (0)

#endif



