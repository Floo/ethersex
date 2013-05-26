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

#ifndef _I2C_ADS1015_H
#define _I2C_ADS1015_H

#define I2C_SLA_ADS1015 0x00

//-- Register --
#define ADS1015_CONV     0x0
#define ADS1015_CONFIG   0x1
#define ADS1015_THR_L    0x2
#define ADS1015_THR_H    0x3

//start communication after reset wit setConfig to initialize

uint8_t i2c_ads1015_writeReg(uint8_t address, uint8_t reg,  uint16_t data);
uint8_t i2c_ads1015_readReg(uint8_t address, uint8_t reg, uint16_t *data);

//all values for Threshold-Registers right adjusted (Ranges from 0x0000 to 0x0FFF)
//all Values read and written from/to 16 bit Registers (except Config-Register) are right shifted by 4 bits

#define i2c_ads1015_setConfig(address, data) i2c_ads1015_writeReg(address, ADS1015_CONFIG, data) //write Config Register
#define i2c_ads1015_setTHRL(address, data) i2c_ads1015_writeReg(address, ADS1015_THR_L, data) //write Low-Threshold
#define i2c_ads1015_setTHRH(address, data) i2c_ads1015_writeReg(address, ADS1015_THR_H, data) //write Low-Threshold
#define i2c_ads1015_getConfig(address, data) i2c_ads1015_readReg(address, ADS1015_CONFIG, pnt_data) //read Config Register

//Reads the Conversion-Register and formates right adjusted (Range from 0x0000 to 0x0FFF)
uint16_t i2c_ads1015_read(uint8_t address); //read Conversation-Register

#endif /* _I2C_ADS1015_H */