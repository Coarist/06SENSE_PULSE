//-----------------------------------------------------------------------------
// File:   CRTOS2.h
// Author: chi
//
// Created on March 5, 2020, 2:05 PM
//-----------------------------------------------------------------------------

#ifndef CRTOS2_H
#define	CRTOS2_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
//-----------------------------------------------------------------------------
// Per task +1 byte RAM, +1 code word FLASH ROM
//-----------------------------------------------------------------------------
#define CRTOS2_NUMBER_TASKS (3)
    
//-----------------------------------------------------------------------------
// CRTOS2 uses TMR0 commonly available on PIC MCUs. There are two cases:
//     (1) TMR0 has no interrupt and TMR0 code is part of the main program flow
//     (2) TMR0 has interrupt and ISR is supplied by CRTOS
// If on the target MCU TMR0 interrupt is available, do the following: 
//     Uncomment the code block immediately below
//     Comment the next code block.
// For MCU models that has not TMR0 interrupt, do the opposite
//     Comment the code block immediately below
//     Uncomment the next code block.
//-----------------------------------------------------------------------------
#ifndef _TMR0INTERRUPT
#define _TMR0INTERRUPT
#endif
//-----------------------------------------------------------------------------
// Uncomment the following block when TMR0 interrupt is not available
//-----------------------------------------------------------------------------
//#ifdef _TMR0INTERRUPT
//#undef _TMR0INTERRUPT
//#endif

//=============================================================================
// Users do not need to configure code beyond this point in this file. 
//=============================================================================

//-----------------------------------------------------------------------------
// This macro expands into 3 bytes RAM, 17 words FLASH ROM (PRO mode)
//-----------------------------------------------------------------------------
#define CRTOS2TimerTick()                                \
{                                                        \
    unsigned char *ii = (unsigned char*)crtos2TaskTimer; \
    while (ii < &crtos2TaskTimer[CRTOS2_NUMBER_TASKS])   \
    {                                                    \
        if(*ii){--*ii;}                                  \
        ++ii;                                            \
    }                                                    \
}

//-----------------------------------------------------------------------------
// Macro that expands to a statement
//-----------------------------------------------------------------------------
#define CRTOS2NextTask(t) {if((++t)>=CRTOS2_NUMBER_TASKS){t=0;}}

//-----------------------------------------------------------------------------
// CRTOS system variables types
//-----------------------------------------------------------------------------
typedef unsigned char CRTOS2_TASK_ID;
typedef unsigned char CRTOS2_T_TIMER;

//-----------------------------------------------------------------------------
// Variables made accessible to the outside world by declaring 'extern'
//-----------------------------------------------------------------------------
extern volatile CRTOS2_T_TIMER crtos2TaskTimer[];

//-----------------------------------------------------------------------------
// Variables made accessible to the outside world by declaring 'extern'
//-----------------------------------------------------------------------------
#ifdef _TMR0INTERRUPT
    #define CRTOS2delayGet(taskno,varName)                          \
        {                                                           \
            T0IE = 0;                                               \
            varName = crtos2TaskTimer[(taskno)];                    \
            T0IE = 1;                                               \
        }
    #define CRTOS2delaySet(taskno,varName)                          \
        {                                                           \
            T0IE = 0;                                               \
            crtos2TaskTimer[(taskno)] = varName;                    \
            T0IE = 1;                                               \
        }
#else
    #define CRTOS2delayGet(taskno,varName)                          \
        {                                                           \
            varName = crtos2TaskTimer[(taskno)];                    \
        }
    #define CRTOS2delaySet(taskno,varName)                          \
        {                                                           \
            crtos2TaskTimer[(taskno)] = varName;                    \
        }
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* CRTOS2_H */

//----------------------------------------------------------------- end of file
