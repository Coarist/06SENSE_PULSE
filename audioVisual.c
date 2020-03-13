//-----------------------------------------------------------------------------
// File:   audioVisual.c
// Author: chi
//
// Created on February 25, 2020, 1:39 PM
// Adapt from PIC18 BCBSP to PIC12 06SENSE_PULSE 13Mar2020
//-----------------------------------------------------------------------------

#include "xc.h"
#include "audioVisual.h"
#include "stdbool.h"
#include "DeviceConfig.h"
#include "CRTOS2.h"

static avControlStruct_t avc[AV_MAX];
#define ONETENTHSECOND (4)

//-----------------------------------------------------------------------------
// Configure the designated AV device to one of the standard output mode.
// Function return value:
//     true  == success
//     false == failed
// t0, t1 scales to clock tick rate implemented in the control code module. 
// Propose 25 ms, x4 = 100 ms. 
//-----------------------------------------------------------------------------
bool __section("audVis") avControl(uint8_t item, uint8_t mode)
{
    if (item >= AV_MAX) {return false;}
    avControlStruct_t *c = &avc[item];
    switch(mode)
    {
        case AV_OFF: 
            c->t0 = 0;
            c->t1 = 1;
            break;
        case AV_PSL:
            c->t0 = (uint8_t)( 0.25 * ONETENTHSECOND);
            c->t1 = (uint8_t)(19.75 * ONETENTHSECOND);  
            break;
        case AV_PSS:
            c->t0 = (uint8_t)(5 * ONETENTHSECOND);
            c->t1 = (uint8_t)(5 * ONETENTHSECOND);  
            break;
        case AV_PRP:
            c->t0 = (uint8_t)(2.5 * ONETENTHSECOND);
            c->t1 = (uint8_t)(2.5 * ONETENTHSECOND);
            break;
        case AV_FUL:
            c->t0 = 1;
            c->t1 = 0;            
            break;
        default:
            return false;
            break;
    }
    return true;
}

void __section("audVis") av_control_task_init(void)
{
    //---------------------------------------------------------------
    // System health indicator uses the in-circuit red LED
    //---------------------------------------------------------------
    avControl(LED_INTERNAL_RED , AV_PSS);
    return;
}

//-----------------------------------------------------------------------------
// Task that centrally controls every attached available audio-visual device.
//-----------------------------------------------------------------------------
CRTOS2_T_TIMER __section("audVis") av_control_task(void)
{
    //---------------------------------------------------------------
    // LED (written this way for low latency and save RAM)
    //---------------------------------------------------------------
    ++avc[0].count;
    if (!avc[0].onOff)
    {
        if (avc[0].t0 && (!avc[0].t1 || (avc[0].count > avc[0].t1)))
        {
            avc[0].count = 0;
            avc[0].onOff = true;
            LED_INTERNAL = 1;
        }
    }
    else
    {
        if (avc[0].t1 && (!avc[0].t0 || (avc[0].count > avc[0].t0))) 
        {
            avc[0].count = 0; 
            avc[0].onOff = false;
            LED_INTERNAL = 0;
        }
    }
    
    //---------------------------------------------------------------
    // Switched +12V output
    //---------------------------------------------------------------
    ++avc[1].count;
    if (!avc[1].onOff)
    {
        if (avc[1].t0 && (!avc[1].t1 || (avc[1].count > avc[1].t1)))
        {
            avc[1].count = 0;
            avc[1].onOff = true;
            OUTPUT_12V = 1;
        }
    }
    else
    {
        if (avc[1].t1 && (!avc[1].t0 || (avc[1].count > avc[1].t0))) 
        {
            avc[1].count = 0; 
            avc[1].onOff = false;
            OUTPUT_12V = 0;
        }
    }
    
    //---------------------------------------------------------------
    // Task wait 25 OS ticks
    //---------------------------------------------------------------
    return 25;
}

#undef ONETENTHSECOND

//----------------------------------------------------------------- end of file
