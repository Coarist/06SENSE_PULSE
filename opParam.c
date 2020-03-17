//-----------------------------------------------------------------------------
// File:   opParam.c
// Author: chi
//
// Created on February 21, 2020, 6:39 PM
// Adapt from PIC18 BCBSP to PIC12 06SENSE_PULSE 13Mar2020
//-----------------------------------------------------------------------------

#include "xc.h"
#include "opParam.h"
#include "alarm.h"
#include "EE12F675.h"
#include "main.h"
#include "CRTOS2.h"
#include "DeviceConfig.h"

//-----------------------------------------------------------------------------
// Compiler directives (XC8) to initialize EEPROM upon programming. Use this
// mechanism to pre-configure the chip for specific application/customer.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 4-byte 32-bit values, pre-alarm and main-alarm levels == 0 == no alarm
// order is least significant byte first, most significant byte on higher addr.
//-----------------------------------------------------------------------------
__EEPROM_DATA(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
//-----------------------------------------------------------------------------
// 0x32 == dec 50 x 10ms sampling interval.
//-----------------------------------------------------------------------------
__EEPROM_DATA(0x32,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);

//-----------------------------------------------------------------------------
// Functions that take a snapshot sample of the current pulse period and apply
// it to alarm (setting the alarm threshold) at the same time updating internal
// parameter EEPROM. 
//-----------------------------------------------------------------------------
void __section("opParam") opSetPre_AlarmFromCapture(void)
{
    uint32_t x;
    GIE = 0;
    x = pulseInterval.value;
    GIE = 1;
    opSetPre_AlarmByValue(x);
    return;
}
void __section("opParam") opSetMainAlarmFromCapture(void)
{
    uint32_t x;
    GIE = 0;
    x = pulseInterval.value;
    GIE = 1;
    opSetMainAlarmByValue(x);
    return;
}
void __section("opParam") opSetPre_AlarmByValue(uint32_t x)
{
    pAlarmLevel = x;
    DATAEE_WriteByte(EA_PALARM+0, x & 0xFF);
    DATAEE_WriteByte(EA_PALARM+1, (x >> 8) & 0xFF);
    DATAEE_WriteByte(EA_PALARM+2, (x >> 16) & 0xFF);
    DATAEE_WriteByte(EA_PALARM+3, (x >> 24) & 0xFF);
    //---------------------------------------------------------------
    // Read-back check is not implemented due to yet-to-be-determined
    // what to do if fail, such as marking an alternative address and
    // write there instead.
    //---------------------------------------------------------------
    return;
}
void __section("opParam") opSetMainAlarmByValue(uint32_t x)
{
    mAlarmLevel = x;
    DATAEE_WriteByte(EA_MALARM+0, x & 0xFF);
    DATAEE_WriteByte(EA_MALARM+1, (x >> 8) & 0xFF);
    DATAEE_WriteByte(EA_MALARM+2, (x >> 16) & 0xFF);
    DATAEE_WriteByte(EA_MALARM+3, (x >> 24) & 0xFF);
    return;
}
void __section("opParam") opZeroAllAlarmLevels(void)
{
    opSetPre_AlarmByValue(0);
    opSetMainAlarmByValue(0);
    return;
}

//-----------------------------------------------------------------------------
// Functions that load operational parameters from internal EEPROM.
//-----------------------------------------------------------------------------
void __section("opParam") opSetPre_AlarmFromEE(void)
{
    uinteger32_t x;
    x.bytes.C0 = DATAEE_ReadByte(EA_PALARM+0);
    x.bytes.C1 = DATAEE_ReadByte(EA_PALARM+1);
    x.bytes.C2 = DATAEE_ReadByte(EA_PALARM+2);
    x.bytes.C3 = DATAEE_ReadByte(EA_PALARM+3);
    pAlarmLevel = x.value;
    return;
}
void __section("opParam") opSetMainAlarmFromEE(void)
{
    uinteger32_t x;
    //EE_READ_BYTE_FROM_EE_INTO_VAR(EA_MALARM+0,x.bytes.C0);
    //EE_READ_BYTE_FROM_EE_INTO_VAR(EA_MALARM+1,x.bytes.C1);
    //EE_READ_BYTE_FROM_EE_INTO_VAR(EA_MALARM+2,x.bytes.C2);
    //EE_READ_BYTE_FROM_EE_INTO_VAR(EA_MALARM+3,x.bytes.C3);
    x.bytes.C0 = DATAEE_ReadByte(EA_MALARM+0);
    x.bytes.C1 = DATAEE_ReadByte(EA_MALARM+1);
    x.bytes.C2 = DATAEE_ReadByte(EA_MALARM+2);
    x.bytes.C3 = DATAEE_ReadByte(EA_MALARM+3);
    mAlarmLevel = x.value;
    return;
}

void __section("opParam") opSetAlarmSamplingInterval(uint8_t x)
{
    DATAEE_WriteByte(EA_SAMPLE, sampleInterval = x);
    return;
};

void __section("opParam") opSetAlarmSamplingFromEE(void)
{
    sampleInterval = DATAEE_ReadByte(EA_SAMPLE);
    return;
};

//-----------------------------------------------------------------------------
// Due to up to 6 ms write time to EEPROM, this 'task' is implemented .
//-----------------------------------------------------------------------------
static uint8_t i = 0;

CRTOS2_T_TIMER __section("opParam") set_threshold_task(void)
{
    static uinteger32_t x;
    
    switch(i)
    {
        case 0: goto SET_THRES_TASK_0; break;
        case 1: goto SET_THRES_TASK_1; break;
        case 2: goto SET_THRES_TASK_2; break;
        case 3: goto SET_THRES_TASK_3; break;
        case 4: goto SET_THRES_TASK_4; break;
        case 5: goto SET_THRES_TASK_5; break;
        default: i = 0; return 100; break;
    }
    //---------------------------------------------------------------
    SET_THRES_TASK_0: 
        if (SET_BUTTON) {return 100;}  //------------- wait key press
        GIE = 0; x.value = pulseInterval.value; GIE = 1;
        mAlarmLevel = x.value;
        ++i;
        DATAEE_WriteByte(EA_MALARM+0, x.bytes.C0);
    SET_THRES_TASK_1:
        if (EECON1bits.WR) {return 3;}
        ++i;
        DATAEE_WriteByte(EA_MALARM+1, x.bytes.C1);
    SET_THRES_TASK_2:
        if (EECON1bits.WR) {return 3;}
        ++i;
        DATAEE_WriteByte(EA_MALARM+2, x.bytes.C2);
    SET_THRES_TASK_3:
        if (EECON1bits.WR) {return 3;}
        ++i;
        DATAEE_WriteByte(EA_MALARM+3, x.bytes.C3);    
    SET_THRES_TASK_4:
        if (EECON1bits.WR) {return 3;}
        ++i;
    SET_THRES_TASK_5: //---------------------------- wait key release
        if (!SET_BUTTON) {i = 0;}
        return 100 - 24;
}

//----------------------------------------------------------------- end of file