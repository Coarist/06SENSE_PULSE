//-----------------------------------------------------------------------------
// File:   alarm.h
// Author: chi
//
// Created on February 25, 2020, 11:57 AM
// Adapt from PIC18 BCBSP to PIC12 06SENSE_PULSE 13Mar2020
//-----------------------------------------------------------------------------

#include <xc.h>
#include "alarm.h"
#include "stdint.h"
#include "stdbool.h"
#include "audioVisual.h"
#include "uintegers2.h"
#include "stdint.h"
#include "main.h"

uint32_t pAlarmLevel = 0;
uint32_t mAlarmLevel = 0;
uint8_t sampleInterval = 50;
static uint8_t samplingCount;

void __section("alarm") alarm_init(void)
{
    samplingCount = 0;
    pAlarmLevel = mAlarmLevel = 0;
    avControl(LED_INTERNAL_RED, AV_PSL);
    avControl(  OUTPUT_SWITCH , AV_OFF);
    return;
}

CRTOS2_T_TIMER __section("alarm") alarm_task(void)
{
    if(++samplingCount < sampleInterval) {return 10;} //---- OS delay
    samplingCount = 0;
    uint32_t x;
    
    GIE=0;
    x = pulseInterval.value;
    GIE=1;
    
    if (x < mAlarmLevel)
    {
        avControl(LED_INTERNAL_RED, AV_FUL);
        avControl(  OUTPUT_SWITCH , AV_FUL);
    }
    else if (x < pAlarmLevel)
    {
        avControl(LED_INTERNAL_RED, AV_PSS);
        avControl(  OUTPUT_SWITCH , AV_PRP);
    }
    else
    {
        if (tmr1Overflowed32 || (x > 2000000))
            {avControl(LED_INTERNAL_RED, AV_OFF);} 
        else
            {avControl(LED_INTERNAL_RED, AV_PSL);}
        avControl(  OUTPUT_SWITCH , AV_OFF);
    }
    return 10; //----- number of OS ticks delay
}

//----------------------------------------------------------------- end of file
