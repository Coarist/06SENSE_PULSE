//-----------------------------------------------------------------------------
// Project:  06SENSE_PULSE
// File:     main.c
// Author:   Coarist
// Dates:    05 Mar 2020
// Compiler: Microchip XC8 (v1.42))
// Target:   06SENSE PIC12F675 (MCC not supported)
//-----------------------------------------------------------------------------

#include <xc.h>
#include "DeviceConfig.h"
#include "CRTOS2.h"
#include "uintegers2.h"
#include "stdint.h"
#include "stdbool.h"
#include "alarm.h"
#include "audioVisual.h"
#include "opParam.h"

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
// 16-bit timer 1 uses these variables to perform 32-bit counting
//-----------------------------------------------------------------------------
static volatile uint32_t tmr1Hi16;
volatile bool tmr1Overflowed32 = false;

//-----------------------------------------------------------------------------
// Sampling (forgone FIFO noise rejection scheme, too resource-intensive)
//-----------------------------------------------------------------------------
volatile uinteger32_t pulseInterval;

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
    // 16-bit timer1
    //---------------------------------------------------------------
    if (TMR1IF)
    {
        if(++tmr1Hi16==0) {tmr1Overflowed32=true;}
        TMR1IF = 0;
    }
    //---------------------------------------------------------------
    // Port Change Interrupt Flag bit
    //---------------------------------------------------------------
    if (GPIF)
    {
        //-----------------------------------------------------------
        // Write (read-modify-write) to port to clear the mismatch
        //-----------------------------------------------------------
        INPUT_PULSE = 0;
        //-----------------------------------------------------------
        // In case pulse edges are very rapid, we are now immediately
        // ready to catch the next edge of opposite polarity. 
        //-----------------------------------------------------------
        GPIF = false;
        //-----------------------------------------------------------
        // Stopping Timer1 is un-avoidable. Make the duration short. 
        //-----------------------------------------------------------
        TMR1ON = false;
        //-----------------------------------------------------------
        // Snapshot lower 16-bit value.
        //-----------------------------------------------------------
        pulseInterval.bytes.C1 = TMR1H;
        pulseInterval.bytes.C0 = TMR1L;
        TMR1H = 0;
        TMR1L = 0; //--- 10 instruction cycle approximated correction
        //-----------------------------------------------------------
        // Release the timer to let it run to time the next edge.  
        //-----------------------------------------------------------
        TMR1ON = true;
        //-----------------------------------------------------------
        // TMR1 ISR code has to be repeated here in case the timer
        // overflowed before it is turned off. 
        //-----------------------------------------------------------
        if (TMR1IF) 
        {
            if(++tmr1Hi16==0) {tmr1Overflowed32=true;}
            TMR1IF =0;
        }
        //-----------------------------------------------------------
        // Record the higher 16-bit value.
        //-----------------------------------------------------------
        if (tmr1Overflowed32)
        {
            pulseInterval.value = UINT32_MAX;
            tmr1Overflowed32 = false;
        }
        else
        {
            pulseInterval.words.W1 = tmr1Hi16;
        }
        //-----------------------------------------------------------
        // Timer 1 higher 16-bit value can now be cleared to zero
        // after it has been referenced. Within this ISR no 16-bit 
        // carry-over will happen hence it is safe to clear it after
        // timer 1 has been turned back on quite a few lines above. 
        //-----------------------------------------------------------
        tmr1Hi16 = 0;
    }
    //---------------------------------------------------------------
    // PIC12F675 interrupt flags not used in this application are:
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
    T1CON  = T1CON_CONF;

    //---------------------------------------------------------------
    // Configure timer 1 (16-bit value comprising two bytes. Since 
    // our target MCU is 8-bit, reading the timer is not a one-cycle
    // operation. Beware of this. 
    //---------------------------------------------------------------
    TMR1ON = TMR1IE = 1;    

    //---------------------------------------------------------------
    // Code modules initializations
    //---------------------------------------------------------------
    av_control_task_init();
    alarm_init();
    opSetMainAlarmFromEE();
    opSetAlarmSamplingFromEE();
    
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
            case 0: t = alarm_task();         break;
            case 1: t = av_control_task();    break;
            case 2: t = set_threshold_task(); break;
            default: continue; //------ bypass CRTOS2delaySet() below
        }
        
        CRTOS2delaySet(i,t);
    }
}

//----------------------------------------------------------------- end of file
