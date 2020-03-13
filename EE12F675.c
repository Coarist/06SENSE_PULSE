//-----------------------------------------------------------------------------
// File:   EE12F675.h
// Author: chi
//
// Created on March 13, 2020, 1:04 PM
// Adapt from MCC generated code for PIC18. On the PIC18 there are more EEPROM
// space and address is 10-bit (held by a uint16_t variable). The EEADRH 
// register is present. On the PIC12F675 EEPROM address only requires 8-bit and
// there is no such register. 
//-----------------------------------------------------------------------------

#include <xc.h>
#include "EE12F675.h"
#include "stdint.h"

//-----------------------------------------------------------------------------
// Write to internal EEPROM:
//     (1) Will block when waiting for the write operation to complete
//     (2) Requires interrupt to be temporarily suspended
//-----------------------------------------------------------------------------
void DATAEE_WriteByte(uint8_t bAdd, uint8_t bData)
{
    EE_WRITE_BYTE(bAdd, bData);
    return;
}

uint8_t DATAEE_ReadByte(uint8_t bAdd)
{
    EEADR = bAdd;
    EECON1bits.RD = 1;
    return (EEDATA);
}

//----------------------------------------------------------------- end of file
