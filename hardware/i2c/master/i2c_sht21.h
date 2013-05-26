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

#define I2C_SLA_SHT21 0x00

//-- Register --
#define SHT21_TRIGGER_T  0xF3
#define SHT21_TRIGGER_RH 0xF5
#define SHT21_WRITE_UREG 0xE6
#define SHT21_READ_UREG  0xE7
#define SHT21_UREG_DEFAULT 0x2
#define SHT21_ADDRESS    0x80

uint16_t i2c_sht21_read(uint8_t reg);
#define i2c_sht21_readTemp() i2c_sht21_read(SHT21_TRIGGER_T) //read temperature register
#define i2c_sht21_readHumid() i2c_sht21_read(SHT21_TRIGGER_RH) //read humidity register

//Set Resolution of the Sensor
//(0 - RH: 12bit, T: 14bit)
//(1 - RH: 8bit, T: 12bit)
//(2 - RH: 10bit, T: 13bit)
//(3 - RH: 11bit, T: 11bit)
uint8_t i2c_sht_setRes(uint8_t value);

#endif /* _I2C_SHT21_H */