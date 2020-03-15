
//-----------------------------------------------------------------------------
// File:   uarts.c
// Author: chi
//
// Created on March 14, 2020, 1:08 PM
// Software UART using CRTOS2 (beware of baud limitation) 
//-----------------------------------------------------------------------------
// TX is implemented as state machines. Each call to the statement machine is 
// a clock tick thus it must be done in exact time interval (which gives
// baud).
//-----------------------------------------------------------------------------
// RX should ideally be implemented using the interrupt-on-pin-change as the 
// event source.
//-----------------------------------------------------------------------------

#include <xc.h>
#include "uarts.h"
#include "CRTOS2.h"
#include "stdint.h"
#include "stdbool.h"
#include "DeviceConfig.h"

//-----------------------------------------------------------------------------
// User to allocate IO pin(s) and to specify the baud rate control factor. 
//-----------------------------------------------------------------------------
#define UARTS_TX_PIN (NOT_USED_3)
#define UARTS_RX_PIN (NOT_USED_3)
#define UARTS_BAUD_N (10)

//-----------------------------------------------------------------------------
// bitCountTx: machine state. 
//     0 == idle and ready
//     1 == launch
//     Non-zero is "busy". 
//-----------------------------------------------------------------------------
static uint8_t bitCountTx, dataByteTx;
static uint8_t bitCountRx, dataByteRx;

bool isUartsTxReady() {return (bitCountTx==0);}
bool isUartsRxReady() {return (bitCountRx==0);}
bool uartsTxLoadByte(uint8_t b) {dataByteTx = b; bitCountTx = 1;}
uint8_t uartsRxGetByte(void) {bitCountRx = 1; return dataByteRx;}

//-----------------------------------------------------------------------------
// (8,N,1) UART transmission TX state machine. Each call to this function 
// advance 1 transmission bit time slot. 
//-----------------------------------------------------------------------------
CRTOS2_T_TIMER uarts_tx_state_machine(void)
{
    switch(bitCountTx)
    {
        case 0: return 0; break;
        case 1:  goto UARTS_TX_0; break;
        case 2:  goto UARTS_TX_1; break;
        case 3:  goto UARTS_TX_1; break;
        case 4:  goto UARTS_TX_1; break;
        case 5:  goto UARTS_TX_1; break;
        case 6:  goto UARTS_TX_1; break;
        case 7:  goto UARTS_TX_1; break;
        case 8:  goto UARTS_TX_1; break;
        case 9:  goto UARTS_TX_1; break;
        case 10: goto UARTS_TX_2; break;
        default: bitCountTx =0; return 0; break;
    }
    //-------------------------------------------------------------------------
    
    UARTS_TX_0: //----------------------------------------- start bit
    UARTS_TX_PIN = 0;
    ++bitCountTx;
    return UARTS_BAUD_N;
    UARTS_TX_1: //----------------------------------------- data bits
    UARTS_TX_PIN = (dataByteTx & 1);
    dataByteTx >>= 1;
    ++bitCountTx;
    return UARTS_BAUD_N;
    UARTS_TX_2: //------------------------------------------ stop bit
    UARTS_TX_PIN = 1;
    ++bitCountTx;
    return UARTS_BAUD_N;
}

//-----------------------------------------------------------------------------
// UART receive state machine. Each call to this function advance 1 receive
// bit time slot. Best to use in conjunction with interrupt-on-pin-change
// if the target MCU has the IOC feature. Note uartsRxGetByte() initializes
// the uarts_rx_state_machine().
//-----------------------------------------------------------------------------
CRTOS2_T_TIMER uarts_rx_state_machine(void)
{
    switch(bitCountRx)
    {
        case 0: return 0; break;
        case 1:  goto UARTS_RX_0; break;
        case 2:  goto UARTS_RX_1; break;
        case 3:  goto UARTS_RX_1; break;
        case 4:  goto UARTS_RX_1; break;
        case 5:  goto UARTS_RX_1; break;
        case 6:  goto UARTS_RX_1; break;
        case 7:  goto UARTS_RX_1; break;
        case 8:  goto UARTS_RX_1; break;
        case 9:  goto UARTS_RX_1; break;
        case 10: goto UARTS_RX_2; break;
        default: bitCountRx=1; return 0; break;
    }
    //-------------------------------------------------------------------------
    
    UARTS_RX_0: //----------------------------------------- start bit
    if (!UARTS_RX_PIN) {++bitCountRx;}
    return UARTS_BAUD_N;
    UARTS_RX_1: //----------------------------------------- data bits
    dataByteTx <<= 1;
    if (UARTS_RX_PIN) {dataByteTx |= 1;}
    ++bitCountRx;
    return UARTS_BAUD_N;
    UARTS_RX_2: //------------------------------------------ stop bit
    if (UARTS_RX_PIN)
        {bitCountRx = 0;} //------- received byte is ready to be read
    else
        {bitCountRx = 1;} //--- error, discard and wait new start bit
    return 0;
}

//----------------------------------------------------------------- end of file
