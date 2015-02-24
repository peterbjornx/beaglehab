/* 
 * This file is part of barod.
 *
 * barod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * barod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with barod.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	24/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __ms5607_h__
#define __ms5607_h__

#define MS5607_ADDR		(0x77)

#define	MS5607_RESET		(0x1E)
#define	MS5607_CV_BARO_4K	(0x48)
#define	MS5607_CV_BARO_2K	(0x46)
#define	MS5607_CV_BARO_1K	(0x44)
#define	MS5607_CV_BARO_512	(0x42)
#define	MS5607_CV_BARO_256	(0x40)
#define	MS5607_CV_TEMP_4K	(0x58)
#define	MS5607_CV_TEMP_2K	(0x56)
#define	MS5607_CV_TEMP_1K	(0x54)
#define	MS5607_CV_TEMP_512	(0x52)
#define	MS5607_CV_TEMP_256	(0x50)
#define	MS5607_ADC_READ		(0x00)
#define	MS5607_PROM_READ(AdDr)	(0xA0 | (AdDr & 0xE))

#define MS5607_M_MANUF		(0x00)
#define MS5607_M_C1		(0x01)
#define MS5607_M_C2		(0x02)
#define MS5607_M_C3		(0x03)
#define MS5607_M_C4		(0x04)
#define MS5607_M_C5		(0x05)
#define MS5607_M_C6		(0x06)
#define MS5607_M_CRC		(0x07)

#endif
