//-----------------------------------------------------------------------------
// Project:  06SENSE_PULSE
// File:     main.c
// Author:   Coarist
// Dates:    1105 Mar 2020
// Compiler: Microchip XC8 (v1.42))
// Target:   06SENSE PIC12F675 (MCC not supported)
//-----------------------------------------------------------------------------

#include <xc.h>
#include "DeviceConfig.h"
#include "CRTOS2.h"
#include "uintegers2.h"
#include "stdint.h"
#include "stdbool.h"

//-----------------------------------------------------------------------------
// Compile time check to ensure the target is compatible with this code.
//-----------------------------------------------------------------------------
#ifndef __12F675
#error "Incorrect MCU target. This code is written for PIC12F675."
#endif

//------------------------------------------------------------------------------
// ID locations 2000h-2003h
// We put in "puls". Only Least Significant 7 bits is stored (ASCII).
//------------------------------------------------------------------------------
__IDLOC7('p','u','l','s');

//-----------------------------------------------------------------------------
// System clock tick using TMR0.
//    Adjust TMR0_RELOAD to tune system clock tick rate
//    Timer ticks: To be tuned by the person who is writing this source code. 
//    Example here is 4MHz FOSC, 1mS TIMR0 interrupt
//-----------------------------------------------------------------------------
#define OPTION_TMR0 (0b11010010)
#define TMR0_RELOAD (125)

//-----------------------------------------------------------------------------
// User to put this in a header file which will be #include'd into this file
//-----------------------------------------------------------------------------
CRTOS2_T_TIMER task0(void);

//-----------------------------------------------------------------------------
// Counters for tmr1 to track 
//-----------------------------------------------------------------------------
uinteger32_t pulseInterval;
static uint32_t tmr1Hi16;
static bool tmr1Overflowed32 = 0;

//-----------------------------------------------------------------------------
// The PIC12F675 services all interrupt sources in one ISR. 
//-----------------------------------------------------------------------------
void interrupt IntVector( void )
{
    if (T0IF)
    {
        TMR0 -= TMR0_RELOAD;
        CRTOS2TimerTick();
        T0IF = 0;
    }
    //---------------------------------------------------------------
    // Additional interrupt servicing IOC, timer1
    //---------------------------------------------------------------
    if (TMR1IF)
    {
        if(++tmr1Hi16==0) {tmr1Overflowed32=1;}
        TMR1IF = 0;
    }
    //---------------------------------------------------------------
    // Port Change Interrupt Flag bit
    //---------------------------------------------------------------
    if (GPIF)
    {
        //; All interrupts are disabled
        //MOVF TMR1H, W ; Read high byte
        //MOVWF TMPH ;
        //MOVF TMR1L, W ; Read low byte
        //MOVWF TMPL ;
        //MOVF TMR1H, W ; Read high byte
        //SUBWF TMPH, W ; Sub 1st read with 2nd read
        //BTFSC STATUS,Z ; Is result = 0
        //GOTO CONTINUE ; Good 16-bit read
        //;
        //; TMR1L may have rolled over between the read of the high and low bytes.
        //; Reading the high and low bytes now will read a good value.
        //;
        //MOVF TMR1H, W ; Read high byte
        //MOVWF TMPH ;
        //MOVF TMR1L, W ; Read low byte
        //MOVWF TMPL ;
        //; Re-enable the Interrupt (if required)
        //CONTINUE ; Continue with your code       
        GPIF = 0;
    }
    
    //---------------------------------------------------------------
    // Other interrupt flags not used in this application are:
    //---------------------------------------------------------------
    // INTF: GP2/INT External Interrupt Flag bit
    // EEIF: EEPROM Write Operation Interrupt Flag bit
    // ADIF: A/D Converter Interrupt Flag bit (PIC12F675 only)
    // CMIF: Comparator Interrupt Flag bit
    //---------------------------------------------------------------

}

//-----------------------------------------------------------------------------
// main() has OS scheduler hard-coded into it. 
//-----------------------------------------------------------------------------
void main(void)
{ 
    //---------------------------------------------------------------
    // Device initialization
    //---------------------------------------------------------------
    OPTION_REG = OPTION_WPU & OPTION_TMR0;
    GIE = 0;
    
    //---------------------------------------------------------------
    // Initializing GPIO (as digital IO, in this case)
    // Refer "PC12F629/675 data sheet DS41190G chapter 3.0 GPIO PORT"
    //---------------------------------------------------------------
    GPIO   = 0b00000000;
    CMCON  = 0b00000111;   // comparator off, lower power consumption
    ANSEL  = 0b00000000;   // ADC turned off.
    TRISIO = TRISIO_CFG;
    WPU    = WPU_CONFIG;   // additional, set up weak pull-up
    IOC    = IOC_CONFIG;

    //---------------------------------------------------------------
    // Cnnfigure timer 1 (16-bit value comprising two bytes. Since 
    // our target MCU is 8-bit, reading the timer is not a one-cycle
    // operation. Beware of this. 
    //---------------------------------------------------------------
    TMR1ON = TMR1IE = 1;    
    
    //---------------------------------------------------------------
    // Enable interrupts of interest (those that are utilized)
    //---------------------------------------------------------------
    INTCON = _INTCON_T0IE_MASK | _INTCON_GIE_MASK | _INTCON_PEIE_MASK;
    
    //---------------------------------------------------------------
    // OS variables
    //---------------------------------------------------------------
    CRTOS2_TASK_ID i;
    CRTOS2_T_TIMER t;

    //---------------------------------------------------------------
    // CRTOS2 task dispatcher infinite loop
    //---------------------------------------------------------------
    for(;;)
    {
        CLRWDT();
        CRTOS2NextTask(i);
        CRTOS2delayGet(i,t);
        
        if (t) {continue;} //----- continue the outer for(;;) loop
        switch(i)
        {
            //-------------------------------------------------------
            // User to add one case for each task (in this style)
            //-------------------------------------------------------
            case 0: t = task0(); break;
            default: continue; //------ bypass CRTOS2delaySet() below
        }
        CRTOS2delaySet(i,t);
    }
}

//----------------------------------------------------------------- end of file
