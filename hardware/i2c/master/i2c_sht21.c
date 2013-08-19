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
#include <stdlib.h>

#include "autoconf.h"
#include "config.h"
#include "core/debug.h"
#include "i2c_master.h"
#include "i2c_sht21.h"

#ifdef I2C_SHT21_SUPPORT


uint16_t i2c_sht21_read_reg(uint8_t reg, uint8_t res)
{
	uint8_t ret = 0xFF;
	uint8_t cnt = 20;
	uint16_t tmp;
	
	if(i2c_sht21_set_resolution(res) != 0) return 0xFFFF;
	
	if (!i2c_master_select(SHT21_ADDRESS, TW_WRITE)) goto end;
	TWDR = reg;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	
	while (cnt-- > 0)
	{
		_delay_ms(5);
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
uint8_t i2c_sht21_set_resolution(uint8_t value)
{
	uint8_t ret = 0xFF;
	uint8_t tmp;
	//first we must read value from user register -> see data sheet, paragraph 5.6
	if (!i2c_master_select(SHT21_ADDRESS, TW_WRITE)) goto end;
	TWDR = SHT21_READ_UREG;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	if (!i2c_master_select(SHT21_ADDRESS, TW_READ)) goto end;
	if (i2c_master_transmit() != TW_MR_DATA_NACK) goto end; //without ACK!! see Fig. 18 in data sheet
	tmp = TWDR;
	//modify value and write back to user register
	tmp = (tmp & 0x81) | value;
	if (!i2c_master_select(SHT21_ADDRESS, TW_WRITE)) goto end;
	TWDR = SHT21_WRITE_UREG;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
	TWDR = tmp;
	if (i2c_master_transmit_with_ack() != TW_MT_DATA_ACK) goto end;
#ifdef DEBUG_I2C
	debug_printf("I2C: SHT21 user register set to 0x%X\n", TWDR);
#endif
	ret = 0;
	end: i2c_master_stop();
	return ret;
}

uint8_t i2c_sht21_read(char *strData, uint8_t type)
{
	int32_t data32;
	int16_t data16;
	uint8_t length;
	if(type == SHT21_TEMP)
	{
		data32 = (int32_t)i2c_sht21_read_reg(SHT21_TRIGGER_T, SHT21_UREG_RH8_T12);
		if(data32 == 0xFFFF) return SHT21_ERROR;
		data32 = SHT21_TEMP_C2 * data32 - SHT21_TEMP_C1;
	}else{
		data32 = (int32_t)i2c_sht21_read_reg(SHT21_TRIGGER_RH, SHT21_UREG_RH12_T14);
		if(data32 == 0xFFFF) return SHT21_ERROR;
		data32 = SHT21_HUMID_C2 * data32 - SHT21_HUMID_C1;
	}
	data16 = (int16_t)(data32 >> 16); //value with 2 digits after decimal point
	if (data16 > 0) //round()
	{
		data16 += 5;
	}else{
		data16 -= 5;
	}
    if(abs(data16) < 100)
    {
        if(data16 >= 0)
        {        
            length = snprintf_P(strData, SHT21_LEN, PSTR("%03d"), abs(data16));
        }else{
            length = snprintf_P(strData, SHT21_LEN, PSTR("-%03d"), abs(data16));
        }
    }else{
        length = snprintf_P(strData, SHT21_LEN, PSTR("%d"), data16);
    }
    strData[length - 1] = strData[length - 2];
    strData[length - 2] = '.';
	return SHT21_OK;
}

#endif /* I2C_SHT21_SUPPORT */