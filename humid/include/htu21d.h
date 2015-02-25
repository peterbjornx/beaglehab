/* 
 * This file is part of humid.
 *
 * humid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * humid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with humid.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	25/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __htu21d_h__
#define __htu21d_h__

#define	HTU21D_ADDR		(0x40)

#define	HTU21D_TRIG_TEMP_HM	(0xE3)
#define	HTU21D_TRIG_HUMI_HM	(0xE5)
#define	HTU21D_TRIG_TEMP_RB	(0xF3)
#define	HTU21D_TRIG_HUMI_RB	(0xF5)
#define	HTU21D_WRITE_UREG	(0xE6)
#define	HTU21D_READ_UREG	(0xE7)
#define	HTU21D_RESET		(0xFE)

#endif
