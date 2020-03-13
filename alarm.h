//-----------------------------------------------------------------------------
// File:   alarm.h
// Author: chi
//
// Created on February 25, 2020, 11:57 AM
// Adapt from PIC18 BCBSP to PIC12 06SENSE_PULSE 13Mar2020
//-----------------------------------------------------------------------------

#ifndef ALARM_H
#define	ALARM_H

#include "stdint.h"
#include "stdbool.h"
#include "CRTOS2.h"

void alarm_init(void);
CRTOS2_T_TIMER alarm_task(void);

extern uint32_t pAlarmLevel;
extern uint32_t mAlarmLevel;
extern uint8_t sampleInterval;

#endif	/* ALARM_H */

//----------------------------------------------------------------- end of file
