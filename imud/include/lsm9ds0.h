/* 
 * This file is part of imud.
 *
 * imud is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * imud is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with imud.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	23/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __lsm9ds0_h__
#define __lsm9ds0_h__

/* 
 * Slave addresses for the sensor
 */
#define LSM9DS0_XM_ADDR		(0x1D)
#define LSM9DS0_G_ADDR		(0x6B)

/* 
 * Magic numbers for the sensor
 */
#define LSM9DS0_G_MAGIC		(0xD4)

/* 
 * Register addresses for the gyroscope
 */
#define LSM9DS0_WHO_AM_I_G	(0x0F)
#define LSM9DS0_CTRL_REG1_G	(0x20)
#define LSM9DS0_CTRL_REG2_G	(0x21)
#define LSM9DS0_CTRL_REG3_G	(0x22)
#define LSM9DS0_CTRL_REG4_G	(0x23)
#define LSM9DS0_CTRL_REG5_G	(0x24)
#define LSM9DS0_REFERENCE_G	(0x25)
#define LSM9DS0_STATUS_REG_G	(0x27)
#define LSM9DS0_OUT_X_L_G	(0x28)
#define LSM9DS0_OUT_X_H_G	(0x29)
#define LSM9DS0_OUT_Y_L_G	(0x2A)
#define LSM9DS0_OUT_Y_H_G	(0x2B)
#define LSM9DS0_OUT_Z_L_G	(0x2C)
#define LSM9DS0_OUT_Z_H_G	(0x2D)
#define LSM9DS0_FIFO_CTRL_REG_G	(0x2E)
#define LSM9DS0_FIFO_SRC_REG_G	(0x2F)
#define LSM9DS0_INT1_CFG_G	(0x30)
#define LSM9DS0_INT1_SRC_G	(0x31)
#define LSM9DS0_INT1_TSH_ZH_G	(0x32)
#define LSM9DS0_INT1_TSH_ZL_G	(0x33)
#define LSM9DS0_INT1_TSH_ZH_G	(0x34)
#define LSM9DS0_INT1_TSH_ZL_G	(0x35)
#define LSM9DS0_INT1_TSH_ZH_G	(0x36)
#define LSM9DS0_INT1_TSH_ZL_G	(0x37)
#define LSM9DS0_INT1_DURATION_G	(0x38)

/*
 * Register addresses for the accelerometer/magnetometer
 */
#define	LSM9DS0_OUT_TEMP_L_XM	(0x05)
#define	LSM9DS0_OUT_TEMP_H_XM	(0x06)
#define	LSM9DS0_STATUS_REG_M	(0x07)
#define	LSM9DS0_OUT_X_L_M	(0x08)
#define	LSM9DS0_OUT_X_H_M	(0x09)
#define	LSM9DS0_OUT_Y_L_M	(0x0A)
#define	LSM9DS0_OUT_Y_H_M	(0x0B)
#define	LSM9DS0_OUT_Z_L_M	(0x0C)
#define	LSM9DS0_OUT_Z_H_M	(0x0D)
#define LSM9DS0_WHO_AM_I_XM	(0x0F)
#define LSM9DS0_INT_CTRL_REG_M	(0x12)
#define LSM9DS0_INT_SRC_REG_M	(0x13)
#define LSM9DS0_INT_THS_L_M	(0x14)
#define LSM9DS0_INT_THS_H_M	(0x15)
#define LSM9DS0_OFFSET_X_L_M	(0x16)
#define LSM9DS0_OFFSET_X_H_M	(0x17)
#define LSM9DS0_OFFSET_Y_L_M	(0x18)
#define LSM9DS0_OFFSET_Y_H_M	(0x19)
#define LSM9DS0_OFFSET_Z_L_M	(0x1A)
#define LSM9DS0_OFFSET_Z_H_M	(0x1B)
#define LSM9DS0_REFERENCE_X	(0x1C)
#define LSM9DS0_REFERENCE_Y	(0x1D)
#define LSM9DS0_REFERENCE_Z	(0x1E)
#define LSM9DS0_CTRL_REG0_XM	(0x1F)
#define LSM9DS0_CTRL_REG1_XM	(0x20)
#define LSM9DS0_CTRL_REG2_XM	(0x21)
#define LSM9DS0_CTRL_REG3_XM	(0x22)
#define LSM9DS0_CTRL_REG4_XM	(0x23)
#define LSM9DS0_CTRL_REG5_XM	(0x24)
#define LSM9DS0_CTRL_REG6_XM	(0x25)
#define LSM9DS0_CTRL_REG7_XM	(0x26)
#define LSM9DS0_STATUS_REG_A	(0x27)
#define LSM9DS0_OUT_X_L_A	(0x28)
#define LSM9DS0_OUT_X_H_A	(0x29)
#define LSM9DS0_OUT_Y_L_A	(0x2A)
#define LSM9DS0_OUT_Y_H_A	(0x2B)
#define LSM9DS0_OUT_Z_L_A	(0x2C)
#define LSM9DS0_OUT_Z_H_A	(0x2D)
#define LSM9DS0_FIFO_CTRL_REG	(0x2E)
#define LSM9DS0_FIFO_SRC_REG	(0x2F)
#define LSM9DS0_INT_GEN_1_REG	(0x30)
#define LSM9DS0_INT_GEN_1_SRC	(0x31)
#define LSM9DS0_INT_GEN_1_THS	(0x32)
#define LSM9DS0_INT_GEN_1_DUR	(0x33)
#define LSM9DS0_INT_GEN_2_REG	(0x34)
#define LSM9DS0_INT_GEN_2_SRC	(0x35)
#define LSM9DS0_INT_GEN_2_THS	(0x36)
#define LSM9DS0_INT_GEN_2_DUR	(0x37)
#define LSM9DS0_CLICK_CFG	(0x38)
#define LSM9DS0_CLICK_SRC	(0x39)
#define LSM9DS0_CLICK_THS	(0x3A)
#define LSM9DS0_TIME_LIMIT	(0x3B)
#define LSM9DS0_TIME_LATENCY	(0x3C)
#define LSM9DS0_TIME_WINDOW	(0x3D)
#define LSM9DS0_ACT_THS		(0x3E)
#define LSM9DS0_ACT_DUR		(0x3F)

#endif
