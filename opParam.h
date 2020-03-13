//-----------------------------------------------------------------------------
// File:   opParam.h
// Author: chi
//
// Created on February 21, 2020, 6:38 PM
// Adapt from PIC18 BCBSP to PIC12 06SENSE_PULSE 13Mar2020
//-----------------------------------------------------------------------------

#ifndef OPPARAM_H
#define	OPPARAM_H

#include "stdint.h"
#include "uintegers2.h"
#include "CRTOS2.h"

//-----------------------------------------------------------------------------
// DATAEE_WriteByte() 
//     It will not return until a write has completed. 
// DATAEE_ReadByte() 
//     "Non-blocking since it does not have to wait for EEPROM to complete the
//     operation. Reading from EEPROM is fairly straight forward.
//-----------------------------------------------------------------------------
// EEPROM map: Internal on-chip EEPROM is allocated as the #define below which
//     denotes the starting addresses of each allocation block.
//-----------------------------------------------------------------------------
#define EA_PALARM (0)
#define EA_MALARM (EA_PALARM+4)
#define EA_SAMPLE (EA_MALARM+4) 
#define EA_CMPVTH (EA_SAMPLE+1)
#define EA_NEXT   (EA_CMPVTH+1)

void opSetPre_AlarmByValue(uint32_t);
void opSetPre_AlarmFromCapture(void);
void opSetMainAlarmByValue(uint32_t);
void opSetMainAlarmFromCapture(void);
void opZeroAllAlarmLevels(void);
void opSetPre_AlarmFromEE(void);
void opSetMainAlarmFromEE(void);
void opSetAlarmSamplingInterval(uint8_t);
void opSetAlarmSamplingFromEE(void);

CRTOS2_T_TIMER set_threshold_task(void);
        
#endif	/* OPPARAM_H */

//----------------------------------------------------------------- end of file
