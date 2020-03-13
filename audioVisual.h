//-----------------------------------------------------------------------------
// File:   audioVisual.h
// Author: chi
//
// Created on February 25, 2020, 1:38 PM
// Adapt from PIC18 BCBSP to PIC12 06SENSE_PULSE 13Mar2020
//-----------------------------------------------------------------------------

#ifndef AUDIOVISUAL_H
#define	AUDIOVISUAL_H

#include "stdint.h"
#include "stdbool.h"
#include "CRTOS2.h"

//-----------------------------------------------------------------------------
// To simplify implementation concerning outputs such as LED, contacts and 
// buzzer, 4 types of pre-defined behaviors are put in place. 
//-----------------------------------------------------------------------------
#define AV_OFF      (0)  // Not turned on
#define AV_PSL      (1)  // Slow, short, periodic pulsation
#define AV_PSS      (2)  // Slow but even M-S periodic pulsation
#define AV_PRP      (3)  // Rapid, periodic pulsation
#define AV_FUL      (4)  // Active continously
#define AV_MODE_MAX (5)

//-----------------------------------------------------------------------------
// Code project 06SENSE_PULSE, in this application the available IOs are 
// mapped in DeviceConfig.h.
//-----------------------------------------------------------------------------
#define LED_INTERNAL_RED  (0)  //-- on-board internal LED
#define OUTPUT_SWITCH     (1)  //-- MOSFET switched out + supply voltage
#define AV_MAX            (2)  //-- Do not delete this line, add entries above.

//-----------------------------------------------------------------------------
// Structure to hold per-channel AV run-time control data
//-----------------------------------------------------------------------------
typedef struct avControlStruct
{
    uint8_t t0; // 0..254  'ON' time. 0 == full off (t1 non-zero)
    uint8_t t1; // 0..254 'OFF' time. 0 == full on  (t0 non-zero)
    uint8_t count;
    bool onOff;
}
avControlStruct_t;

bool avControl(uint8_t item, uint8_t mode);
void av_control_task_init(void);
CRTOS2_T_TIMER av_control_task(void);

#endif	/* AUDIOVISUAL_H */

//----------------------------------------------------------------- end of file
