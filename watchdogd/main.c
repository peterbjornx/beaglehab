/* 
 * This file is part of watchdogd.
 *
 * watchdogd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * watchdogd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with watchdogd.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	22/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

int main( int argc, char **argv ) 
{
	
	for ( ; ; ) {

		wdipc_process ( );
		pm_process ( );
		
	
	}


}
