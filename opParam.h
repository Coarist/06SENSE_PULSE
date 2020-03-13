//-----------------------------------------------------------------------------
// File:   opParam.h
// Author: chi
//
// Created on February 21, 2020, 6:38 PM
//-----------------------------------------------------------------------------
// The PIC18F26K22 has 1024 bytes of individually addressable individually 
// writable EEPROM. Representation of the 8-bit on-chip EEPROM address 
// registers EEADR and EEADRH are expressed together as a single 16-bit
// unsigned integer. For 1024 bytes, the address range is 0x0000 through to
// 0x03FF. 
//-----------------------------------------------------------------------------

#ifndef OPPARAM_H
#define	OPPARAM_H

#include "stdint.h"
#include "uintegers2.h"

//-----------------------------------------------------------------------------         
// Logging such as overspeed will ideally have date-time stamp which will need
// real-time clock. 
// - possibly events due to abrupt deceleration (hard braking)
// - will require RTC
// - will be logged to external IIC EEPROM
//-----------------------------------------------------------------------------

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
void opSetCmpVoltThresholdByValue(uint8_t);
void opSetCmpVoltThresholdFromEE(void);

#endif	/* OPPARAM_H */

//----------------------------------------------------------------- end of file
