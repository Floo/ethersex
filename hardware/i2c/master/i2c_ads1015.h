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

#define ADS1015_OK 1
#define ADS1015_ERROR 0

#define I2C_SLA_ADS1015 0x48 //same as DS1631!

//-- Register --
#define ADS1015_CONV     0x00
#define ADS1015_CONFIG   0x01
#define ADS1015_THR_L    0x02
#define ADS1015_THR_H    0x03

//-- Bitmask --
#define ADS1015_REG_CONFIG_OS       0x8000
#define ADS1015_REG_CONFIG_MUX_AIN0 0x4000
#define ADS1015_REG_CONFIG_MUX_AIN1 0x5000
#define ADS1015_REG_CONFIG_MUX_AIN2 0x6000
#define ADS1015_REG_CONFIG_MUX_AIN3 0x7000
#define ADS1015_REG_CONFIG_PGA_6_144 0x0000
#define ADS1015_REG_CONFIG_PGA_4_096 0x0200
#define ADS1015_REG_CONFIG_PGA_2_048 0x0400
#define ADS1015_REG_CONFIG_PGA_1_024 0x0600
#define ADS1015_REG_CONFIG_PGA_0_512 0x0800
#define ADS1015_REG_CONFIG_PGA_0_256 0x0A00
#define ADS1015_REG_CONFIG_DEFAULT  0x01C3

//start communication after reset wit setConfig to initialize

uint8_t i2c_ads1015_write_reg(uint8_t address, uint8_t reg,  uint16_t data);
uint8_t i2c_ads1015_read_reg(uint8_t address, uint8_t reg, uint16_t *data);

//all values for Threshold-Registers right adjusted (Ranges from 0x0000 to 0x0FFF)
//all Values read and written from/to 16 bit Registers (except Config-Register) are right shifted by 4 bits

#define i2c_ads1015_set_config(address, data) i2c_ads1015_write_reg(address, ADS1015_CONFIG, data) //write Config Register
#define i2c_ads1015_set_THRL(address, data) i2c_ads1015_write_reg(address, ADS1015_THR_L, data) //write Low-Threshold
#define i2c_ads1015_set_THRH(address, data) i2c_ads1015_write_reg(address, ADS1015_THR_H, data) //write Low-Threshold
#define i2c_ads1015_get_config(address, pnt_data) i2c_ads1015_read_reg(address, ADS1015_CONFIG, pnt_data) //read Config Register

//Reads the Conversion-Register and formates right adjusted (Range from 0x0000 to 0x0FFF)
uint16_t i2c_ads1015_read(uint8_t address); //read Conversation-Register
uint8_t i2c_ads1015_conversion_ready(uint8_t address); //check if conversion is finished
uint8_t i2c_ads1015_start_conversion(uint8_t address, uint8_t input, uint8_t gain); //switch Mux to input and PGA to gain and start single conversion

#endif /* _I2C_ADS1015_H */