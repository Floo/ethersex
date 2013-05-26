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

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#include "autoconf.h"
#include "config.h"
#include "core/debug.h"
#include "i2c_master.h"
#include "i2c_ads1015.h"

#ifdef I2C_ADS1015_SUPPORT

uint8_t i2c_ads1015_writeReg(uint8_t address, uint8_t reg,  uint16_t data)
{
	uint8_t ret = 0xFF;
	if (!i2c_master_select(address, TW_WRITE)) goto end;
	TWDR = reg;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	if (reg == ADS1015_CONFIG) {
		TWDR = (unsigned char)((data >> 8) & 0xFF); //MSB first
	}else{
		TWDR = (unsigned char)((data >> 12) & 0xFF); //MSB first
	}
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	if (reg == ADS1015_CONFIG) {
		TWDR = (unsigned char)(data & 0xFF); //LSB
	}else{
		TWDR = (unsigned char)((data >> 4) & 0xFF); //LSB
	}
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
#ifdef DEBUG_I2C
	debug_printf("I2C: i2c_ads1015_writeReg: Reg 0x%X: MSB 0x%X, LSB 0x%X\n", reg, (unsigned char)((data >> 8) & 0xFF), (data & 0xFF));
#endif
	ret = 0;
	end: i2c_master_stop();
	if (ret == 0){
		ret = i2c_ads1015_setConfigToConv();
	}
	return ret;
}

uint8_t i2c_ads1015_setConfigToConv(uint8_t address)
{
	uint8_t ret = 0xFF;
	if (!i2c_master_select(address, TW_WRITE)) goto end;
	TWDR = ADS1015_CONV;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	ret = 0;
	end: i2c_master_stop();
	return ret;
}

uint8_t i2c_ads1015_readReg(uint8_t address, uint8_t reg, uint16_t *data)
{
	uint8_t ret = 0xFF;
	uint8_t tmp[2];
	if (!i2c_master_select(address, TW_WRITE)) goto end;
	TWDR = reg;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	i2c_master_stop();
	
	if (!i2c_master_select(address, TW_READ)) goto end;
	if (i2c_master_transmit_with_ack() != TW_MR_DATA_ACK) goto end;
	tmp[1] = TWDR; //MSB
	if (i2c_master_transmit() != TW_MR_DATA_NACK) goto end;
	tmp[0] = TWDR; //LSB
#ifdef DEBUG_I2C
	debug_printf("I2C: i2c_ads1015_readReg: Reg 0x%X: MSB 0x%X, LSB 0x%X\n", reg, tmp[1], tmp[0]);
#endif

	ret = 0;
	*data = ((tmp[1] << 8) | tmp[0]);
	end: i2c_master_stop();
	if (ret == 0){
		ret = i2c_ads1015_setConfigToConv();
	}	
	return ret;
}

//Reads the Conversion-Register and formates right adjusted (Range from 0x0000 to 0x0FFF)
uint16_t i2c_ads1015_read(uint8_t address)
{
	uint8_t ret = 0xFF;
	uint16_t tmp;
	
	if (!i2c_master_select(address, TW_READ)) goto end;
	if (i2c_master_transmit_with_ack() != TW_MR_DATA_ACK) goto end;
	tmp = TWDR << 4; //MSB
	if (i2c_master_transmit() != TW_MR_DATA_NACK) goto end;
	tmp = tmp | (TWDR >> 4); //LSB

	ret = 0;
	end: if (ret == 0) {
		#ifdef DEBUG_I2C
			debug_printf("I2C: ADS1015 read value 0x%X\n", tmp);
		#endif
		return tmp;
	}else{
		return 0xFFFF;
	}
}

#endif /* I2C_ADS1015_SUPPORT */