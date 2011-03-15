/*
*
* Copyright (c) 2010 by Jens Prager <jprager@online.de>
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


#include "rec868.h"

#ifndef FS20_SEND_SUPPORT
    #define DEBUG_PB1_ENABLE
#endif



volatile struct rec868_global_t rec868_global;
uip_ipaddr_t udp_ip;
extern uint16_t udp_count;

/******* empfangene FS20-Sequenz in Ausgabeformat umwandeln *********/

void out_FS20(void)
{
    uint8_t len;
    char *buf;

    if(rec868_global.stat.fs20)
    {
        if (!(rec868_global.fsseq[3] & 0x20)){  //Erweiterundbit nicht gesetzt
            rec868_global.fsseq[4] = 0;
        }
        len = 52;
        buf = malloc(len);
        snprintf(buf, len, "%05u FS20 HC1:%03d HC2:%03d ADDR:%03d CMD:%03d EW:%03d\n", udp_count++, rec868_global.fsseq[0], rec868_global.fsseq[1],
                    rec868_global.fsseq[2], rec868_global.fsseq[3], rec868_global.fsseq[4]);
        if (buf != 0)
            uecmd_sender_send_command(&udp_ip, buf, NULL);
    }
}

/******* empfangene Wetter-Sequenz in Ausgabeformat umwandeln *********/

void out_Wett(void)
{
    //txWett[]: Temp[2], Wind[2], Regen[2], Feuchte[1], RegenSofort[1]
    uint8_t len;
    char *buf;
    int16_t temp;
    uint16_t wind;
    uint16_t regen;
    uint8_t feuchte;
    uint8_t regenSofort;

    if(rec868_global.stat.wett)
    {
        temp = rec868_global.wsseq[2] + 10 * rec868_global.wsseq[3] + 100 * rec868_global.wsseq[4];
        if (rec868_global.wsseq[1] & 0x08)  //neg. Temperatur
        {
            temp *= -1;
        }
        wind = rec868_global.wsseq[7] + 10 * rec868_global.wsseq[8] + 100 * rec868_global.wsseq[9];
        regen = rec868_global.wsseq[12];
        regen <<= 4;
        regen |= rec868_global.wsseq[11];
        regen <<= 4;
        regen |= rec868_global.wsseq[10];
        feuchte = rec868_global.wsseq[5] + 10 * rec868_global.wsseq[6]; // Feuchte
        regenSofort = rec868_global.wsseq[1] & 0x02;
        len = 62;
        buf = malloc(len);
        snprintf(buf, len, "%05u KS300 TEMP:%+04d WIND:%03d RAINCOUNT:%04d HUMID:%02d RAIN:%01d\n", udp_count++,
                 temp, wind, regen, feuchte, regenSofort);
        if (buf != 0)
            uecmd_sender_send_command(&udp_ip, buf, NULL);
    }
}

/******* empfangene Helligkeitssequenz in Ausgabeformat umwandeln *********/

void out_Hell(void)
{
    uint8_t len;
    char *buf;
    int16_t hell, val2;

    if(rec868_global.stat.hell)
    {
        hell = rec868_global.wsseq[2] + 10 * rec868_global.wsseq[3] + 100 * rec868_global.wsseq[4];
        if (rec868_global.wsseq[1] & 0x08)  //neg. Wert
        {
            hell *= -1;
        }
        val2 = rec868_global.wsseq[5] + 10 * rec868_global.wsseq[6] + 100 * rec868_global.wsseq[7];
        len = 34;
        buf = malloc(len);
        snprintf(buf, len, "%05u AS2000 TEMP:%+04d HUMID:%03d\n", udp_count++, hell, val2);
        if (buf != 0)
            uecmd_sender_send_command(&udp_ip, buf, NULL);
    }
}


void rec868_process(void)
{
    uint8_t i = 0;
    uint8_t sum;
    uint8_t chk = 0;

    if (rec868_global.kommando == REC868_REC_WETT)
    {
        #ifdef DEBUG_REC868
        debug_printf("wssend: %d\n", rec868_global.wssend);
        #endif
        sum = 5;
        do
        {
            chk ^= rec868_global.wsseq[i];
            sum += rec868_global.wsseq[i++];
            if (i == rec868_global.wssend - 2)
            {
                if (chk != rec868_global.wsseq[rec868_global.wssend - 2])
                {
                    // Checksummmenfehler
                    #ifdef DEBUG_REC868
                    debug_printf("Fehler in XOR-Checksumme: %d erwartet aber %d empfangen \n",
                                 chk, rec868_global.wsseq[rec868_global.wssend - 2]);
                    #endif
                    i = 0;
                    rec868_start();
                    break;
                }
            }
            if (i == rec868_global.wssend - 1)
            {
                if ((sum & 0x0F) != rec868_global.wsseq[rec868_global.wssend - 1])
                {
                    // Checksummmenfehler
                    #ifdef DEBUG_REC868
                    debug_printf("Fehler in SUM-Checksumme: %d erwartet aber %d empfangen \n",
                                 sum & 0x0F, rec868_global.wsseq[rec868_global.wssend - 1]);
                    #endif
                    i = 0;
                    rec868_start();
                    break;
                }
            }
        }
        while(i < rec868_global.wssend - 1);

        if (i > 0)
        {
            if (((rec868_global.wsseq[0] & 0x07) == 7) && rec868_global.stat.wett)  // Typ == KS300
            {
                #ifdef DEBUG_REC868
                debug_printf("KS300_Sequenz an UDP-Client senden...\n");
                #endif
                out_Wett();//per UDP rauschicken
            }
            else if (((rec868_global.wsseq[0] & 0x07) == 1) && rec868_global.stat.hell)   // Typ == Thermo/Hygro (AS2000)
            {
                #ifdef DEBUG_REC868
                debug_printf("AS2000_Sequenz an UDP-Client senden...\n");
                #endif
                out_Hell();//per UDP senden
            }
            rec868_start();
            rec868_global.kommando = REC868_CLR;
        }
    }

    if (rec868_global.kommando == REC868_REC_FS20)
    {
        sum = 6;

        do
        {
            sum += rec868_global.fsseq[i++];
        }
        while(i < rec868_global.fssend - 1);
        if ((rec868_global.fsseq[i] == sum) && rec868_global.stat.fs20) //Checksumme und STAT-Flag müssen stimmen
        {
            #ifdef DEBUG_REC868
            debug_printf("FS20_Sequenz an UDP-Client senden...\n");
            #endif
            out_FS20(); //per UDP rausschicken
        }
        else
        {
            #ifdef DEBUG_REC868
            if(rec868_global.stat.fs20)
                debug_printf("Checksummenfehler\n");
            #endif
        }
        rec868_start();
        rec868_global.kommando = REC868_CLR;
    }
}

void rec868_stop(void)
{
    #ifdef DEBUG_PB1_ENABLE
    PORTB &= ~(1<<PB1); //Clear PB1: REC Signalisierung abschalten
    #endif
    ACSR &= ~_BV(ACIE); //Interrupt ANALOG_COMP sperren
}

/******* PinChangeInt für Empfang initialisieren *********/

void rec868_init(void)
{
    //UDP-Client setzen
    uip_ipaddr_copy(&udp_ip, all_ones_addr);
    //uip_ipaddr(&udp_ip, 192, 168, 178, 22);
    //benoetigte Funktionen aktivieren
    rec868_global.stat.fs20 = FALSE;
    rec868_global.stat.wett = FALSE;
    rec868_global.stat.hell = FALSE;

    rec868_global.fssend = 5;
    rec868_global.wssend = 10;
    ic_error(); // Variablen initialisieren
    //Timer0 initialisieren
    TCCR0A = 0; //NormalMode
    TCCR0B = _BV(CS02); //Vorteiler: clk/256
    TIFR0 = 0;
    //PinChangeInt init

    //Pins ueber E6-Fkten festlegen
    //DDRD &= ~(1<<PD6); //Pin PD6 als Eingang
    //PCMSK3 = _BV(PCINT30); //PinChange fuer PCINT30 (PD6) freigeben

    /* configure port pin for use as input to the analoge comparator */
    //DDR_CONFIG_IN(FS20_RECV);
    //PIN_CLEAR(FS20_RECV);
    //Pins ueber E6-Fkten festlegen
    DDRB &= ~(1<<PB3); //Pin PB3 als Eingang
    PORTB &= ~(1<<PB3); //Clear PB3
    #ifdef DEBUG_PB1_ENABLE
    //fuer Tests PB1 als Ausgang
    DDRB |= (1<<PB1);
    PORTB |= (1<<PB1);
    #endif
    /* enable analog comparator,
     * use fixed voltage reference (1V, connected to AIN0)
     * Reaktion auf Flanke je nach ACIS0-Wert
     */
    ACSR = _BV(ACBG) | _BV(ACIS1);

    //rec868_start();
}

/******* Pin-Change-Interrupt für Empfang freigeben *********/

void rec868_start(void)
{
    ic_error(); //Variablen ruecksetzen
    #ifdef DEBUG_PB1_ENABLE
    //PORTB |= (1<<PB1); //Set PB1: REC Signalisierung einschalten
    #endif
    rec868_global.kommando = REC868_CLR;
    ACSR &= ~_BV(ACI); //anstehende Interrupts löschen
    ACSR |= _BV(ACIE); //Enable AnanlogComperatorInterrupt
}


/******* Rücksetzen der Empfangssequenz *********/

void ic_error(void)
{
    #ifdef DEBUG_PB1_ENABLE
    //PORTB &= ~(1<<PB1); //Clear PB1: REC Signalisierung abschalten
    #endif
    rec868_global.fsrec = FALSE;
    rec868_global.wsrec = FALSE;
    rec868_global.fspre = FALSE;
    rec868_global.wspre = FALSE;
    rec868_global.bitcount = 0;
    rec868_global.bytecount = 0;
}

/******* Auswerten des FS20 Datenbits im IC-Interrupt-Handler *********/

void ic_up1(void)
{
    if (rec868_global.bitcount == 8)  // ganzes Byte empfangen
    {
        if (parity_even_bit(rec868_global.recbyte) != (rec868_global.th >= T2)) // Parität checken
        {
            ic_error(); // Paritätsfehler
            return;
        }
        rec868_global.fsseq[rec868_global.bytecount++] = rec868_global.recbyte; // empfangenes Byte speichern
        rec868_global.bitcount = 0;
        if (rec868_global.bytecount == 4)
        {
            rec868_global.fssend = 5;
            if (rec868_global.recbyte & 0x20)
            {
                rec868_global.fssend++; // wenn Erweiterungsbit eingeschoben werden soll
            }
            return;
        }
        if (rec868_global.bytecount == rec868_global.fssend)
        {
            rec868_global.kommando = REC868_REC_FS20; // vollständige FS20-Sequenz empfangen
            rec868_stop(); // Disable AnalogCompInterrupt
            ic_error(); // alles rücksetzen
            #ifdef DEBUG_REC868
            debug_printf("FS20-Sequenz empfangen\n");
            #endif
        }
        return;
    }
    rec868_global.recbyte <<= 1;
    if (rec868_global.th >= T2)
    {
        rec868_global.recbyte |= 1; // 1 reinschieben
    }
    rec868_global.bitcount++;
}


/******* Auswerten des Wetter-Datenbits im IC-Interrupt-Handler *********/

void ic_up2(void)
{
    rec868_global.recbyte >>= 4; //noch 4 Stellen weiterschieben, damit es rechtsbündig steht
    rec868_global.wsseq[rec868_global.bytecount++] = rec868_global.recbyte & 0x0F; // Wetter-Nibble in Empfangssequenz schreiben

    if (rec868_global.bytecount == 1)  // entscheiden, welcher Sender es war
    {
        if ((rec868_global.recbyte & 0x07) == 7)
        {
            // KS300
            rec868_global.wssend = 16; // Anzahle der zu empfangenden Bytes festlegen
        }
        else
        {
            // AS2000
            rec868_global.wssend = 10;
        }
        return;
    }

    if (rec868_global.bytecount == rec868_global.wssend)
    {
        rec868_global.kommando = REC868_REC_WETT; // vollst. Wettersequenz empfangen
        rec868_stop(); // Disable AnalogCompInterrupt
        ic_error(); // alles rücksetzen
        #ifdef DEBUG_REC868
        debug_printf("Wetter-Sequenz empfangen\n");
        #endif
    }
}


/******* Interruptroutine für Analog_Comparator and AIN1 (PB3)
    Comperator bezogen auf Bandgap-Ref. (1V)
	Auswertung der Impulsfolgen *********/

ISR(ANALOG_COMP_vect)
{
    rec868_global.ttemp = TCNT0; //Timerstand auslesen

    if(bit_is_set(TIFR0, TOV2))
    {
        rec868_global.ttemp = 0xFF; //Timer ist bereits uebergelaufen, also auf max setzen
        TIFR0 = _BV(TOV2); //Overflow loeschen
    }
    TCNT0 = 0; //Timer reset
    ACSR ^= _BV(ACIS0); //ausloesende Flanke wechseln
    if (ACSR & _BV(ACIS0))  // L/H-Flanke detektiert: ACIS0 geloescht heisst steigende Flanke,
                            // da im vorherigen Befehl invertiert
    {
        rec868_global.tl = rec868_global.ttemp; //Dauer des vorhergehenden L-Pegels (Sender aus)
        return;
    }
    // H/L-Flanke detektiert
    #ifdef DEBUG_PB1_ENABLE
    //PORTB |= (1<<PB1); //Set PB1: REC Signalisierung einschalten
    #endif
    rec868_global.th = rec868_global.ttemp; //Dauer des H-Pegels (Sender-An-Dauer)
    // Beginn der Auswertung
    if(rec868_global.th < T1)
    {
        //*** Stör-Peak gefunden
        return;
    }
    if (rec868_global.th < T2)
    {
        if (!rec868_global.wsrec)
        {
            //*** FS20: 0 wurde erkannt
            if (!rec868_global.fsrec)
            {
                rec868_global.fsrec = TRUE; // Empfang der FS20-sequenz beginnt
                rec868_global.fspre = TRUE;
            }
            if (rec868_global.fspre)
            {
                rec868_global.bitcount++; // Präambel-Bit empfangen
                return;
            }
            ic_up1();
            return;
            //*** Ende: FS20: 0 wurde erkannt
        }
        //*** Wetterstation: 1 wurde erkannt
        if (rec868_global.th < T5)
        {
            if (rec868_global.wsrec)
            {
                if (rec868_global.wspre)
                {
                    if (rec868_global.bitcount <= 5)
                    {
                        ic_error();
                        return;
                    }
                    rec868_global.wspre = FALSE; // Ende der Präambel
                    rec868_global.bitcount = 0;
                    #ifdef DEBUG_PB1_ENABLE
                    PORTB |= (1<<PB1); //Set PB1: REC Signalisierung einschalten
                    #endif
                    return;
                }
                if (rec868_global.bitcount == 4)
                {
                    rec868_global.bitcount = 0; //Trennzeichen empfangen, neuen Nibble initialisieren
                    return;
                }
                //Datennibble noch nicht voll
                rec868_global.recbyte >>= 1;
                rec868_global.recbyte |= 0x80; // eine 1 von links einschieben, da LSB first
                rec868_global.bitcount++;

                if (rec868_global.bitcount == 4)  // Datennibble ist jetzt voll
                {
                    ic_up2(); // Auswerten
                }
                return;
            }
        }
        //*** Ende: Wetterstation: 1 wurde erkannt
    }
    if (rec868_global.th < T3)
    {
        //*** FS20: 1 wurde erkannt
        if (rec868_global.fsrec)
        {
            if (!rec868_global.fspre)  // 1 kommt in Präambel nicht vor
            {
                ic_up1();
                return;
            }
            if (rec868_global.bitcount > 6)  // Ende der Präambel
            {
                #ifdef DEBUG_PB1_ENABLE
                //PORTB |= (1<<PB1); //Set PB1: REC Signalisierung einschalten
                #endif
                rec868_global.fspre = FALSE;
                rec868_global.bitcount = 0;
                return;
            }
        }
        ic_error();
        return;
        //*** Ende: FS20: 1 wurde erkannt
    }
    if (rec868_global.th < T4)
    {
        //*** Wetterstation: 0 wurde erkannt
        if (!rec868_global.wsrec)
        {
            if (rec868_global.fsrec)
            {
                ic_error();
                return;
            }
            rec868_global.wsrec = TRUE; // Empfang einer Wetterstation-Sequenz beginnt
            rec868_global.wspre = TRUE;
        }
        if (rec868_global.wspre)
        {
            rec868_global.bitcount++; // Präambel-Bit gefunden
            return;
        }
        if (rec868_global.bitcount == 4)  // Trennzeichen darf nicht 0 sein
        {
            ic_error();
            return;
        }
        rec868_global.recbyte >>= 1; // Bit in Bytesequenz von links einschieben
        rec868_global.bitcount++;
        if (rec868_global.bitcount == 4)
        {
            ic_up2(); // Auswerten
        }
        return;
        //*** Ende: Wetterstation: 0 wurde erkannt
    }
    ic_error();
}


/*
  -- Ethersex META --
  header(hardware/radio/fs20/rec868.h)
  mainloop(rec868_process)
  init(rec868_init)
*/
