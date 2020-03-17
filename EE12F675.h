//-----------------------------------------------------------------------------
// File:   EE12F675.h
// Author: chi
//
// Created on March 13, 2020, 1:08 PM
// Adapt from MCC generated code for PIC18. 
//-----------------------------------------------------------------------------

#ifndef EE12F675_H
#define	EE12F675_H

#include <xc.h>
#include "stdint.h"

void DATAEE_WriteByte(uint8_t, uint8_t);
uint8_t DATAEE_ReadByte(uint8_t);

//-------------------------------------------------------------------
// Special sequence: refer PIC12F675 data sheet. To write an EEPROM 
// data location, the user must first write the address to the EEADR 
// register and the data to the EEDATA register. Then the user must 
// follow a specific sequence to initiate the write for each byte.
//-------------------------------------------------------------------
// uint8_t GIEBitValue = INTCONbits.GIE;
// EECON1bits.WREN = 1; //---------------------- Enable write
// INTCONbits.GIE = 0;  //---------------- Disable interrupts
// EECON2 = 0x55;
// EECON2 = 0xAA;
// EECON1bits.WR = 1;
// EECON1bits.WREN = 0; //--------- disable (safe-lock) write
// INTCONbits.GIE = GIEBitValue; //- restore interrupt enable
//-------------------------------------------------------------------
// EEPROM TDEW Erase/Write cycle time can take up to 6 ms. 
//-------------------------------------------------------------------
// Consecutive write has to pull the EE Write Complete Interrupt Flag
// bit (EEIF) is set (or enable this interrupt). The EEIF bit 
// (PIR<7>) register must be cleared by software. Alternatively, the
// WR bit will be cleared by hardware upon completion of write cycle.
//-------------------------------------------------------------------

#define EE_SPECIAL_UNLOCK_SEQ() \
{                               \
    EECON2 = 0x55;              \
    EECON2 = 0xAA;              \
    EECON1bits.WR = 1;          \
}

#define EE_TRANSFER_FROM_SFR_TO_EE() \
{                                    \
    EECON1bits.WREN = 1;             \
    INTCONbits.GIE = 0;              \
    EE_SPECIAL_UNLOCK_SEQ();         \
    EECON1bits.WREN = 0;             \
    INTCONbits.GIE = 1;              \
}

//-------------------------------------------------------------------
// At the completion of the write cycle, the WR bit is cleared in 
// hardware and the EE Write Complete Interrupt Flag bit (EEIF) is 
// set. The user can either enable this interrupt or poll this bit. 
// The EEIF bit (PIR<7>) register must be cleared by software.
//-------------------------------------------------------------------
// For writing to EEPROM, at the moment the code does not perform
// write verifying. 
//-------------------------------------------------------------------

#define EE_WRITE_BYTE(a,d)        \
{                                 \
    EEADR = a; EEDATA = d;        \
    EE_TRANSFER_FROM_SFR_TO_EE(); \
}

//-------------------------------------------------------------------
// After this macro has been expanded and executed, the EEDATA
// register will contain the byte read from address a.
//-------------------------------------------------------------------

#define EE_READ_BYTE_FROM_EE_INTO_SFR(a) \
{                                        \
    EEADR = (a);                         \
    EECON1bits.RD = 1;                   \
}

#define EE_READ_BYTE_FROM_EE_INTO_VAR(a,d) \
{                                          \
    EE_READ_BYTE_FROM_EE_INTO_SFR(a);      \
    d = EEDATA;                            \
}

#endif	/* EE12F675_H */

//----------------------------------------------------------------- end of file
