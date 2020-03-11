//-----------------------------------------------------------------------------
// File:   uintegers2.h
// Author: chi
//
// C:   8-bit  char (byte)
// W:  16-bit  word
// L:  32-bit  long
// 
// Created on 16 February 2020, 10:44
//-----------------------------------------------------------------------------
// stdint.h does not provide 24-bit integral types. For a 8-bit MCU, 24-bit can
// obtain more efficiency. uinteger24_t provides such a storage type together
// with byte-level and word-level access. 
//-----------------------------------------------------------------------------
// Endianess - Definitely works correctly with Little-endian machines since 
//             ordering is clearly cohesive. 
//-----------------------------------------------------------------------------

#ifndef INTEGERS2_H
#define	INTEGERS2_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h" //--------------------- uintxx_t definitive bit-width types
    
//-----------------------------------------------------------------------------
// 24-bit unsigned value with individual byte word access
//-----------------------------------------------------------------------------
typedef union uinteger24
{
    struct {
        uint8_t C0; uint8_t C1; uint8_t C2;
    } bytes;
    struct {
        uint16_t W0; uint8_t C2;
    } wordL;
    struct {
        uint8_t C0; uint16_t WH; 
    } wordH;
}
uinteger24_t;

//-----------------------------------------------------------------------------
// U24 shorthand macros for uinteger24 type. 
//-----------------------------------------------------------------------------
#define U24GETVALUE(x) (((uint32_t)(x.wordH.WH)<<8)|x.bytes.C0)
#define U24ZEROIZE(x) (x.wordH.C0=x.wordH.WH=0)
#define U24SETMAX(x) {x.wordH.WH=0xFFFF;x.bytes.C0=0xFF;}

//-----------------------------------------------------------------------------
// Arithmetic utility: on the PIC18 byte increment methods if done carefully 
// can save execution overhead. The example below is optimization on top of XC8
// compiler 'PRO' mode:
//     if (tmr1Byte2 != UINT8_MAX) ++tmr1Byte2;     // check before increment
//     if (!(++tmr1Byte2)) {tmr1Byte2 = UINT8_MAX;} // increment then check
// The second line saves machine instruction cycles
//     In one of our observations achieved -4 bytes == -2 instructions
// Note it may be more efficient if other operation such as uint8 compare is 
// grouped with these operations in different ways. 
//-----------------------------------------------------------------------------
#define U8_INC_UNTIL_LIMIT_1(x) {if(!(++(x))){x=UINT8_MAX;}}
//-----------------------------------------------------------------------------
// Writing in this form is more resilient to interference from interrupt when
// the ISR contains code that zeroizes the variable. This arises from the need
// to avoid disabling interrupt when accessing the variables - TMR accuracy 
// consideration.                                                     22Feb2020
//-----------------------------------------------------------------------------
#define U8_INC_UNTIL_LIMIT_2(x) {if(((x)!=UINT8_MAX)){++x;}}

//-----------------------------------------------------------------------------
// 32-bit unsigned value with individual byte word access
//-----------------------------------------------------------------------------
typedef union uinteger32
{
    uint32_t value;
    struct {
        uint8_t C0; uint8_t C1; uint8_t C2; uint8_t C3;
    } bytes;
    struct {
        uint16_t W0; uint16_t W1;
    } words;
}
uinteger32_t;

#ifdef	__cplusplus
}
#endif
//------------------------------------------------------------------ end of file
#endif	/* INTEGERS2_H */

