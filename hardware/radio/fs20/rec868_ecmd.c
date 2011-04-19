/*
* Copyright (c) 2010 by Jens Prager <jprager@online.de>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
* For more information on the GPL, please go to:
* http://www.gnu.org/copyleft/gpl.html
*/

#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "protocols/ecmd/ecmd-base.h"
#include "protocols/uip/uip.h"
#include "protocols/uip/parse.h"

#ifdef REC868_SUPPORT
#include "hardware/radio/fs20/rec868.h"
#endif

#ifdef REC868_SUPPORT
int16_t parse_cmd_rec868_fs20_start(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.fs20 = TRUE;
  rec868_start();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_fs20_stop(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.fs20 = FALSE;
  if(!rec868_global.stat.fs20 & !rec868_global.stat.wett & !rec868_global.stat.hell)
    rec868_stop();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_ks300_start(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.wett = TRUE;
  rec868_start();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_ks300_stop(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.wett = FALSE;
  if(!rec868_global.stat.fs20 & !rec868_global.stat.wett & !rec868_global.stat.hell)
    rec868_stop();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_as2000_start(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.hell = TRUE;
  rec868_start();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_as2000_stop(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.hell = FALSE;
  if(!rec868_global.stat.fs20 & !rec868_global.stat.wett & !rec868_global.stat.hell)
    rec868_stop();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_start(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.wett = TRUE;
  rec868_global.stat.hell = TRUE;
  rec868_global.stat.fs20 = TRUE;
  rec868_start();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_stop(char *cmd, char *output, uint16_t len)
{
  rec868_global.stat.wett = FALSE;
  rec868_global.stat.hell = FALSE;
  rec868_global.stat.fs20 = FALSE;
  rec868_stop();
  return ECMD_FINAL_OK;
}

int16_t parse_cmd_rec868_ip(char *cmd, char *output, uint16_t len)
{
    uip_ipaddr_t ip;
    while (*cmd == ' ')
        cmd++;

    if (*cmd != '\0')
    {
        /* try to parse ip */
        if (parse_ip(cmd, &ip) != 0){
              return ECMD_ERR_PARSE_ERROR;
        }
        uip_ipaddr_copy(&udp_ip, &ip);
    }else{
        return ECMD_FINAL(print_ipaddr(&udp_ip, output, len));
    }
    return ECMD_FINAL_OK;
}

#endif //REC868_SUPPORT

/*
-- Ethersex META --
block(REC868)
ecmd_feature(rec868_fs20_start, "rec868 fs20 start",, Start FS20 receiver.)
ecmd_feature(rec868_fs20_stop, "rec868 fs20 stop",, Stop FS20 receiver.)
ecmd_feature(rec868_ks300_start, "rec868 ks300 start",, Start KS300 receiver.)
ecmd_feature(rec868_ks300_stop, "rec868 ks300 stop",, Stop KS300 receiver.)
ecmd_feature(rec868_as2000_start, "rec868 as2000 start",, Start AS2000 receiver.)
ecmd_feature(rec868_as2000_stop, "rec868 as2000 stop",, Stop AS2000 receiver.)
ecmd_feature(rec868_start, "rec868 start",, Start FS20, AS2000 and WS300 receiver.)
ecmd_feature(rec868_stop, "rec868 stop",, Stop FS20, AS2000 and KS300 receiver.)
ecmd_feature(rec868_ip, "rec868 ip",, Set IP for UDP-Client to receive REC868-Commands.)
*/
