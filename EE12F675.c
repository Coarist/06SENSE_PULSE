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
    uint8_t GIEBitValue = INTCONbits.GIE;
    
    //---------------------------------------------------------------
    // Special sequence: refer PIC12F675 data sheet
    //---------------------------------------------------------------
    EECON1bits.WREN = 1; //----------------------------- Enable write
    INTCONbits.GIE = 0;  //----------------------- Disable interrupts
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    EECON1bits.WREN = 0; //---------------- disable (safe-lock) write
    INTCONbits.GIE = GIEBitValue; //-------- restore interrupt enable
    return;
}

uint8_t DATAEE_ReadByte(uint8_t bAdd)
{
    EEADRH = ((bAdd >> 8) & 0x03);
    EEADR = (bAdd & 0xFF);
    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    NOP();  // NOPs may be required for latency at high frequencies
    NOP();

    return (EEDATA);
}

//----------------------------------------------------------------- end of file
