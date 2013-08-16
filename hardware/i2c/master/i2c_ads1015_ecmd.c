/*
 * Copyright (c) 2009 by Christian Dietrich <stettberger@dokucode.de>
 * Copyright (c) 2009 by Stefan Riepenhausen <rhn@gmx.net>
 * Copyright (c) 2012 by Erik Kunze <ethersex@erik-kunze.de>
 * Copyright (c) 2013 by Jens Prager <jprager@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <avr/pgmspace.h>
#include <util/twi.h>
#include <string.h>

#include "autoconf.h"
#include "config.h"
#include "core/debug.h"
#include "hardware/i2c/master/i2c_ads1015.h"
#include "protocols/ecmd/ecmd-base.h"

int16_t parse_cmd_i2c_ads1015_read(char *cmd, char *output, uint16_t len)
{
	uint8_t adr;
	uint8_t input;
	uint8_t gain;
	int16_t result = 0xFFFF;
	uint8_t cnt1 = 6;
	uint8_t cnt2 = 5;
	sscanf_P(cmd, PSTR("%hhu %hhu %hhu"), &adr, &input, &gain);
	if (adr > 3)
		return ECMD_ERR_PARSE_ERROR;
	adr += I2C_SLA_ADS1015;
	if(i2c_ads1015_start_conversion(adr, input, gain) == ADS1015_ERROR)
	{
		return ECMD_FINAL(snprintf_P(output, len, PSTR("error starting conversion")));
	}
	//allow enough time for conversion
	while(cnt1-- > 0)
	{
		cnt2 = 5;
		while(cnt2-- > 0) _delay_us(30);//wait for 150 us
		if(i2c_ads1015_conversion_ready(adr))
		{
			result = i2c_ads1015_read(adr);
			break;
		}
	}
	if (result == 0xFFFF)
	{
		return ECMD_FINAL(snprintf_P(output, len, PSTR("error reading result")));
	}
#ifdef ECMD_MIRROR_REQUEST
  return
    ECMD_FINAL(snprintf_P
               (output, len, PSTR("ads1015 read %d %d %d %d"), adr, input, gain, result));
#else
	return ECMD_FINAL(snprintf_P(output, len, PSTR("%d"), result));
#endif
}

/*
-- Ethersex META --

  block([[I2C]] (TWI))
  ecmd_feature(i2c_ads1015_read, "ads1015 read", ADDR INPUT GAIN, Start single conversion for INPUT(0...3) and GAIN(0->6.144V...5->0.256V))
*/