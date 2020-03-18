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

//-----------------------------------------------------------------------------
// Changed code to alarm at low frequency (large pulse period). Optimized for 
// high execution efficiency. Also added multiple sampling to make alarm more
// steady (user to feel the alarm is more deterministic)              18Mar2020
//-----------------------------------------------------------------------------
CRTOS2_T_TIMER __section("alarm") alarm_task(void)
{
    static uint8_t y;
    uint32_t x;
    static bool a;

    if(++samplingCount < sampleInterval) {return 10;} //---- OS delay
    samplingCount = 0;
    
    GIE=0;
    x = pulseInterval.value;
    GIE=1;
    
    if (x > mAlarmLevel) 
    {
        if (!++y) {y = UINT8_MAX;}
    }
    else
    {
        y = 0;
    }
    
    if (y > 3 && !a)
    {
        a = true;
        avControl(LED_INTERNAL_RED, AV_FUL);
        avControl(  OUTPUT_SWITCH , AV_FUL);
    }
    //else if (x < pAlarmLevel)
    //{
    //    avControl(LED_INTERNAL_RED, AV_PSS);
    //    avControl(  OUTPUT_SWITCH , AV_PRP);
    //}
    else if (!y && a)
    {
        //if (tmr1Overflowed32 || (x > 2000000))
        //    {avControl(LED_INTERNAL_RED, AV_OFF);} 
        //else
        //    {avControl(LED_INTERNAL_RED, AV_PSL);}
        a = false;
        avControl(LED_INTERNAL_RED, AV_PSL);
        avControl(  OUTPUT_SWITCH , AV_OFF);
    }
    return 10; //--------------------------- number of OS ticks delay
}

//----------------------------------------------------------------- end of file
