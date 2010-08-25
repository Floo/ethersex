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

#ifndef _REC868_H
#define _REC868_H


#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/parity.h>

#ifndef AUTOCONF_INCLUDED
#include "autoconf.h"
#endif

#include "config.h"
#include "core/debug.h"

#ifndef TRUE
#define TRUE      1
#endif
#ifndef FALSE
#define FALSE     0
#endif


// Bits des Status-Registers STAT und des Empfangs-Registers REC

#define FS20      7
#define WETT      6
#define HELL      5

// Befehle zur Steuerung des Programmablaufes
#define REC868_CLR      0x00
#define REC868_REC_WETT 0x01 // KS300 oder AS2000 empfangen, steht zum Auswerten bereit
#define REC868_REC_FS20 0x02 // FS20-Sequenz empfangen, üsteht zum Auswerten bereit

// Grenzen zur Unterscheidung der Pulslängen
// Vorteiler: 1 Timerschritt = 12,8µs
// CLK/256
//#define T1	38 // 300 µs
#define T1  16 // 230 µs
#define T2	39 // 500 µs
#define T5  51 //650 µs
#define T3	55 // 700 µs
//#define T4 125 // 1000 µs
#define T4 86 // 1100 µs

struct rec868_global_t
{
    uint8_t kommando;
    //Datenfelder der empfangenen und auszugebenden Werte
    struct
    {
        uint8_t fs20:1;
        uint8_t wett:1;
        uint8_t hell:1;
    }stat;
    union
    {
        uint8_t txWett[8]; //Temp[2], Wind[2], Regen[2], Feuchte[1], RegenSofort[1](bool)
        uint8_t txFS20[3]; //Adresse, Befehl_1, Befehl_2
        uint8_t txHell[2]; //int16
    };
    //Werte zum Auswerten der Empfangssequenzen
    uint8_t fsrec; //FS20-Sequenz wird empfangen (bool)
    uint8_t wsrec; //Wetter-Sequenz wird empfangen (bool)
    uint8_t fspre; //Präambel der FS20-Sequenz wird empfangen (bool)
    uint8_t wspre; //Präambel der Wetter-Sequenz wird empfangen (bool)

    uint8_t th; //Dauer des H-Pegels
    uint8_t tl; //Dauer des L-Pegels
    uint8_t ttemp; //aktueller Timerstand beim Aufruf des Interrupts

    uint8_t bitcount; //Zähler der empfangenen Bits
    uint8_t bytecount; //Zähler der empfangenen Datenbytes

    uint8_t fssend; //Anzahl der zu empfangenden FS20-Datenbytes
    uint8_t wssend; //Anzahl der zu empfangenden Wetter-Datenbytes

    uint8_t recbyte; //Zwischenspeicher zum einschieben der empfangenen Datenbits
    uint8_t wsseq[16]; //empfangene Wetterstationssequenz
    uint8_t fsseq[6]; //empfangene FS20-Sequenz
};

extern volatile struct rec868_global_t rec868_global;

void rec868_process(void);
void rec868_init(void);
void rec868_stop(void);
void start_Rec(void);
void out_FS20(void);
void out_Wett(void);
void out_Hell(void);
void init_Rec(void);
void ic_error(void);
void ic_up1(void);
void ic_up2(void);


#endif //_REC868_H


