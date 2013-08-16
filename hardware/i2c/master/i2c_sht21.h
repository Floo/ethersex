/*
*
* Copyright (c) 2013 by Jens Prager <jprager@gmx.de>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
* For more information on the GPL, please go to:
* http://www.gnu.org/copyleft/gpl.html
*/

#ifndef _I2C_SHT21_H
#define _I2C_SHT21_H

#define I2C_SLA_SHT21 0x80

#define SHT21_OK = 1
#define SHT21_ERROR = 0
#define SHT21_LEN = 6

//-- Register --
#define SHT21_TRIGGER_T  0xF3
#define SHT21_TRIGGER_RH 0xF5
#define SHT21_WRITE_UREG 0xE6
#define SHT21_READ_UREG  0xE7
#define SHT21_ADDRESS    0x80

//Set Resolution of the Sensor
#define SHT21_UREG_RH12_T14 0x00 //(RH: 12bit, T: 14bit)
#define SHT21_UREG_RH8_T12  0x01 //(RH: 8bit, T: 12bit)
#define SHT21_UREG_RH10_T13 0x81 //(RH: 10bit, T: 13bit)
#define SHT21_UREG_RH11_T11 0x81 //(RH: 11bit, T: 11bit)

// see data sheet SHT21 for calculating of results from register values
// given formulae multiplied by 100 to omit floating point operation
// division by 2^16 in last step as bit shift operation and division by 100 during string output 
#define SHT21_TEMP_C1 307036160 //4685 * 2^16
#define SHT21_TEMP_C2 17572 //
#define SHT21_HUMID_C1 39321600 //6 * 100 * 2^16
#define SHT21_HUMID_C2 12500 //125 * 100

#define SHT21_TEMP 0
#define SHT21_HUMID 1

#define i2c_sth21_read_temp(pnt_strData) i2c_sht21_read(pnt_strData, SHT21_TEMP) //read temperature register
#define i2c_sht21_read_humid(pnt_strData) i2c_sht21_read(pnt_strData, SHT21_HUMID) //read humidity register

uint8_t i2c_sht_set_resolution(uint8_t value);
uint16_t i2c_sht21_read_reg(uint8_t reg, uint8_t res);
uint8_t i2c_sht21_read(char *strData, uint8_t type);

#endif /* _I2C_SHT21_H */