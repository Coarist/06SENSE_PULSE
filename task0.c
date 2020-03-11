//-----------------------------------------------------------------------------
// File:   task0.c
// Author: chi
//
// Created on March 5, 2020, 3:25 PM
//-----------------------------------------------------------------------------

#include "CRTOS2.h"

//-----------------------------------------------------------------------------
// Variables, allocation of RAM. i is "task re-entrance index". 
//-----------------------------------------------------------------------------
static uint8_t i = 0;

//-----------------------------------------------------------------------------
// The function return value is the delay (number of OS tick(s)) that the task
// is to wait (suspend execution). Value 0 means returning control to OS but
// the task is immediately 
//-----------------------------------------------------------------------------
CRTOS2_T_TIMER task0(void)
{
    switch(i)
    {
        case 0: goto T0L0; break;
        case 1: goto T0L1; break;
        case 2: goto T0L2; break;
        default: i = 0; return 0; break;
    }

    //--------------------------------------------------------------------------
    T0L0: 
    i = 0;

    
    ++i;
    T0L1: if (1) {return 1;}  //------------ loop
    

    ++i;
    T0L2: if (1) {return 1;}  //------------ loop
    
    
    i = 0;
    return 0;                 //------ outer loop
}

//----------------------------------------------------------------- end of file
