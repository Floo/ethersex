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
#include "i2c_sht21.h"

#ifdef I2C_SHT21_SUPPORT


uint16_t i2c_sht21_read(uint8_t reg)
{
	uint8_t ret = 0xFF;
	uint8_t cnt = 10;
	uint16_t tmp;
	if (!i2c_master_select(SHT21_ADDRESS, TW_WRITE)) goto end;
	TWDR = reg;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	
	while (cnt-- > 0)
	{
		_delay_ms(10);
		if (!i2c_master_select(SHT21_ADDRESS, TW_READ)) continue;
		if (i2c_master_transmit_with_ack() != TW_MR_DATA_ACK) goto end;
		tmp = TWDR << 8; //MSB
		if (i2c_master_transmit() != TW_MR_DATA_NACK) goto end;
		tmp = tmp | TWDR; //LSB
		ret = 0;
		break;
	}
	end: i2c_master_stop();
	if (ret == 0) {
		#ifdef DEBUG_I2C
			debug_printf("I2C: SHT read value 0x%X\n", tmp);
		#endif
		return tmp;
	}else{
		return 0xFFFF;
	}
}

//Set Resolution of the Sensor
//(0 - RH: 12bit, T: 14bit)
//(1 - RH: 8bit, T: 12bit)
//(2 - RH: 10bit, T: 13bit)
//(3 - RH: 11bit, T: 11bit)
uint8_t i2c_sht_setRes(uint8_t value)
{
	uint8_t ret = 0xFF;
	if (!i2c_master_select(SHT21_ADDRESS, TW_WRITE)) goto end;
	TWDR = SHT21_WRITE_UREG;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	TWDR = SHT21_UREG_DEFAULT | (value & 0x01) | ((value << 6) & 0x80);
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
#ifdef DEBUG_I2C
	debug_printf("I2C: SHT user register set to 0x%X\n", TWDR);
#endif
	ret = 0;
	end: i2c_master_stop();
	return ret;
}

#endif /* I2C_SHT21_SUPPORT */