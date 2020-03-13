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

static uint8_t i = 0;

CRTOS2_T_TIMER __section("opParam") set_threshold_task(void)
{
    switch(i)
    {
        case 0: goto T3L0; break;
        case 1: goto T3L1; break;
        default: i = 0; return 100; break;
    }
    //---------------------------------------------------------------
    T3L0: if ( SET_BUTTON) {return 100;}  //---------- wait key press
    ++i;
    opSetMainAlarmFromCapture();
    T3L1: if (!SET_BUTTON) {return 100;} //--------- wait key release
    i = 0;
    return 100;
}

//----------------------------------------------------------------- end of file