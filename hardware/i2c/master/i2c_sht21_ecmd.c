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
#include "hardware/i2c/master/i2c_sht21.h"
#include "protocols/ecmd/ecmd-base.h"

int16_t parse_cmd_i2c_sht21_read_temperature(char *cmd, char *output, uint16_t len)
{
	char strData[SHT21_LEN];

	if(i2c_sht21_read_temp(strData) == SHT21_ERROR)
	{
		return ECMD_FINAL(snprintf_P(output, len, PSTR("error reading value")));
	}
#ifdef ECMD_MIRROR_REQUEST
  return
    ECMD_FINAL(snprintf_P
               (output, len, PSTR("sht21 temp %s"), strData));
#else
	return ECMD_FINAL(snprintf_P(output, len, PSTR("%s"), strData));
#endif
}

int16_t parse_cmd_i2c_sht21_read_humidity(char *cmd, char *output, uint16_t len)
{
	char strData[SHT21_LEN];
	
	if(i2c_sht21_read_humid(strData) == SHT21_ERROR)
	{
		return ECMD_FINAL(snprintf_P(output, len, PSTR("error reading value")));
	}
#ifdef ECMD_MIRROR_REQUEST
  return
    ECMD_FINAL(snprintf_P
               (output, len, PSTR("sht21 humid %s"), strData));
#else
	return ECMD_FINAL(snprintf_P(output, len, PSTR("%s"), strData));
#endif
}

/*
-- Ethersex META --

  block([[I2C]] (TWI))
  ecmd_feature(i2c_sht21_read_temperature, "sht21 temp",, Return temperature)
  ecmd_feature(i2c_sht21_read_humidity, "sht21 humid",, Return humidity)
*/