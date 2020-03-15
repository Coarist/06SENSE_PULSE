//-----------------------------------------------------------------------------
// File:   uarts.h
// Author: chi
//
// Created on March 14, 2020, 1:07 PM
// Software UART using CRTOS2 (beware of baud limitation) 
//-----------------------------------------------------------------------------

#ifndef UARTS_H
#define	UARTS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "CRTOS2.h"
#include "stdint.h"
#include "stdbool.h"

bool isUartsTxReady(void);
bool isUartsRxReady(void);
bool uartsTxLoadByte(uint8_t);
uint8_t uartsRxGetByte(void);
CRTOS2_T_TIMER uarts_tx_state_machine(void);
CRTOS2_T_TIMER uarts_rx_state_machine(void);

#endif	/* UARTS_H */

//----------------------------------------------------------------- end of file
