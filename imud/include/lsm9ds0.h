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

/* Device identification register */
#define LSM9DS0_WHO_AM_I_G	(0x0F)


/* Control register 1 */
#define LSM9DS0_CTRL_REG1_G	(0x20)

/* DR field of CR1G */
#define LSM9DS0_CR1G_DR_95HZ	(0x00)
#define LSM9DS0_CR1G_DR_190HZ	(0x40)
#define LSM9DS0_CR1G_DR_380HZ	(0x80)
#define LSM9DS0_CR1G_DR_760HZ	(0xC0)
#define LSM9DS0_CR1G_DR_MASK	(0xC0)

/* BW field of CR1G */
#define LSM9DS0_CR1G_BW0	(0x00)
#define LSM9DS0_CR1G_BW1	(0x10)
#define LSM9DS0_CR1G_BW2	(0x20)
#define LSM9DS0_CR1G_BW3	(0x30)
#define LSM9DS0_CR1G_BW_MASK	(0x30)

/* Flags of CR1G */
#define LSM9DS0_CR1G_PD		(0x08)
#define LSM9DS0_CR1G_XEN	(0x02)
#define LSM9DS0_CR1G_YEN	(0x01)
#define LSM9DS0_CR1G_XEN	(0x04)

/* Control register 2 */
#define LSM9DS0_CTRL_REG2_G	(0x21)

/* Highpass filter mode */
#define LSM9DS0_CR2G_HP_RNORMAL	(0x00)
#define LSM9DS0_CR2G_HP_REF	(0x10)
#define LSM9DS0_CR2G_HP_NORMAL	(0x20)
#define LSM9DS0_CR2G_HP_AUTORES	(0x30)
#define LSM9DS0_CR2G_HPM_MASK	(0x30)

/* Highpass filter cutoff */
#define LSM9DS0_CR2G_HPCF(PaR)	(0x0F & (PaR))


/* Control register 3 */
#define LSM9DS0_CTRL_REG3_G	(0x22)

/* intg pin is interupt */
#define LSM9DS0_CR3G_IG_INT1	(0x80)

/* intg pin is boot status */
#define LSM9DS0_CR3G_IG_BOOT	(0x40)

/* intg pin is active low */
#define LSM9DS0_CR3G_IG_ACTIVEL	(0x20)

/* outputs are open drain */
#define LSM9DS0_CR3G_OPENDRAIN	(0x10)

/* drdg pin is data ready */
#define LSM9DS0_CR3G_DR_DRDY	(0x08)

/* drdg pin is FIFO watermark IRQ */
#define LSM9DS0_CR3G_DR_FWM	(0x04)

/* drdg pin is FIFO overrun IRQ */
#define LSM9DS0_CR3G_DR_FOVR	(0x02)

/* drdg pin is FIFO empty IRQ */
#define LSM9DS0_CR3G_DR_FEMPT	(0x01)


/* Control register 4 */
#define LSM9DS0_CTRL_REG4_G	(0x23)

/* Only update when both bytes have been read */
#define LSM9DS0_CR4G_BLOCK_UPD	(0x80)

/* Big/Little Endian selection */
#define LSM9DS0_CR4G_BIG_ENDIAN	(0x40)

/* Scale selection */
#define LSM9DS0_CR4G_SCALE_245	(0x00)
#define LSM9DS0_CR4G_SCALE_500	(0x10)
#define LSM9DS0_CR4G_SCALE_2000	(0x01)
#define LSM9DS0_CR4G_SCALE_MASK	(0x11)

/* Self-test mode */
#define LSM9DS0_CR4G_ST_NORMAL	(0x00)
#define LSM9DS0_CR4G_ST_XPYZN	(0x02)
#define LSM9DS0_CR4G_ST_XNYZP	(0x06)
#define LSM9DS0_CR4G_ST_MASK	(0x06)

/* SPI Mode Selection */
#define LSM9DS0_CR4G_SPI_MODE	(0x01)


/* Control register 5 */
#define LSM9DS0_CTRL_REG5_G	(0x24)

/* Reboot memory content */
#define LSM9DS0_CR5G_BOOT	(0x80)

/* FIFO Enable */
#define LSM9DS0_CR5G_FIFO_EN	(0x40)

/* Highpass filter enable */
#define LSM9DS0_CR5G_HPF_EN	(0x10)

/* Interrupt generator source selection */
#define LSM9DS0_CR5G_IS_LPF1	(0x00)
#define LSM9DS0_CR5G_IS_HPF	(0x04)
#define LSM9DS0_CR5G_IS_LPF2	(0x08)
#define LSM9DS0_CR5G_IS_LPF2B	(0x0C)

/* Output selection */
#define LSM9DS0_CR5G_OS_LPF1	(0x00)
#define LSM9DS0_CR5G_OS_HPF	(0x01)
#define LSM9DS0_CR5G_OS_LPF2	(0x02)
#define LSM9DS0_CR5G_OS_LPF2B	(0x03)

/* Reference register ?!?! */
#define LSM9DS0_REFERENCE_G	(0x25)

/* Gyro status register */
#define LSM9DS0_STATUS_REG_G	(0x27)

/* Overrun flags */
#define LSM9DS0_SRG_ZYXOR	(0x80)
#define LSM9DS0_SRG_ZOR		(0x40)
#define LSM9DS0_SRG_YOR		(0x20)
#define LSM9DS0_SRG_XOR		(0x10)

/* Available flags */
#define LSM9DS0_SRG_ZYXDA	(0x08)
#define LSM9DS0_SRG_ZDA		(0x04)
#define LSM9DS0_SRG_YDA		(0x02)
#define LSM9DS0_SRG_XDA		(0x01)

/* Output registers */
#define LSM9DS0_OUT_X_L_G	(0x28)
#define LSM9DS0_OUT_X_H_G	(0x29)
#define LSM9DS0_OUT_Y_L_G	(0x2A)
#define LSM9DS0_OUT_Y_H_G	(0x2B)
#define LSM9DS0_OUT_Z_L_G	(0x2C)
#define LSM9DS0_OUT_Z_H_G	(0x2D)


/* FIFO control register */
#define LSM9DS0_FIFO_CTRL_REG_G	(0x2E)

/* FIFO Modes */
#define	LSM9DS0_CRFG_FM_BYPASS	(0x00)
#define	LSM9DS0_CRFG_FM_FIFO	(0x20)
#define	LSM9DS0_CRFG_FM_STREAM	(0x40)
#define	LSM9DS0_CRFG_FM_STRFIFO	(0x60)
#define	LSM9DS0_CRFG_FM_BYPSTR	(0x80)

/* FIFO Watermark */
#define LSM9DS0_CRFG_WTM(VaL)	(0x1F & VaL)


/* FIFO Source register */
#define LSM9DS0_FIFO_SRC_REG_G	(0x2F)

/* Watermark status */
#define LSM9DS0_FSG_WTM		(0x80)
#define LSM9DS0_FSG_OVRN	(0x40)
#define LSM9DS0_FSG_EMPTY	(0x20)

/* FIFO Stored data level */
#define LSM9DS0_FSG_FSS(VaL)	(0x1F & VaL)

/* Interrupt 1 Configuration Register */
#define LSM9DS0_INT1_CFG_G	(0x30)

/* AND/OR Combination of interrupt events */
#define LSM9DS0_IR1G_AND	(0x80)

/* Latch interrupt request */
#define LSM9DS0_IR1G_LATCH	(0x40)

/* Interrupt on {X,Y,Z}{High,Low} */
#define LSM9DS0_IR1G_ZHIE	(0x20)
#define LSM9DS0_IR1G_ZLIE	(0x10)
#define LSM9DS0_IR1G_YHIE	(0x08)
#define LSM9DS0_IR1G_YLIE	(0x04)
#define LSM9DS0_IR1G_XHIE	(0x02)
#define LSM9DS0_IR1G_XLIE	(0x01)

/* Interrupt source register */
#define LSM9DS0_INT1_SRC_G	(0x31)

/* Interrupt active */
#define LSM9DS0_IS1G_IA		(0x40)

/* Interrupted by {X,Y,Z}{High,Low} */
#define LSM9DS0_IS1G_ZH		(0x20)
#define LSM9DS0_IS1G_ZL		(0x10)
#define LSM9DS0_IS1G_YH		(0x08)
#define LSM9DS0_IS1G_YL		(0x04)
#define LSM9DS0_IS1G_XH		(0x02)
#define LSM9DS0_IS1G_XL		(0x01)

/* Interrupt threshold registers */
#define LSM9DS0_INT1_TSH_ZH_G	(0x32)
#define LSM9DS0_INT1_TSH_ZL_G	(0x33)
#define LSM9DS0_INT1_TSH_ZH_G	(0x34)
#define LSM9DS0_INT1_TSH_ZL_G	(0x35)
#define LSM9DS0_INT1_TSH_ZH_G	(0x36)
#define LSM9DS0_INT1_TSH_ZL_G	(0x37)

/* Interrupt duration register */
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
