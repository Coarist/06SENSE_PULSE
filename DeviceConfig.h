//------------------------------------------------------------------------------
// File:   DeviceConfig.h
// Author: chi
// Created on 21 September 2018, 13:19
// Adapt from 06SENSE to PIC12 06SENSE_PULSE 13Mar2020
//------------------------------------------------------------------------------
// This file contains #pragma config statements should precede project file 
// includes. #include this file make sure it is preceding other #includes.
//------------------------------------------------------------------------------

#ifndef DEVICECONFIG_H
#define	DEVICECONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// CONFIG
//------------------------------------------------------------------------------
// C source code generated by MPLab-X Window>PIC Memory Views>Configuration Bits
//------------------------------------------------------------------------------
// Oscillator Selection bits (INTOSC oscillator:
//     I/O function on GP4/OSC2/CLKOUT pin 
//     I/O function on GP5/OSC1/CLKIN) 
// Watchdog Timer Enable bit (WDT enabled) 
// Power-Up Timer Enable bit (PWRT enabled)
// GP3/MCLR pin function select (GP3/MCLR pin function is I/O) 
// Brown-out Detect Enable bit (BOD enabled)
// Code Protection bit (Program Memory code protection is enabled) 
// Data Code Protection bit (Data memory code protection is disabled)
//------------------------------------------------------------------------------
// Use project enums instead of #define for ON and OFF.
//------------------------------------------------------------------------------
#pragma config FOSC  = INTRCIO   
#pragma config WDTE  = ON        
#pragma config PWRTE = ON       
#pragma config MCLRE = OFF       
#pragma config BOREN = ON       
#pragma config CP    = ON          
#pragma config CPD   = OFF        

//------------------------------------------------------------------------------
// IO port pins
//------------------------------------------------------------------------------
// Allocation (PIC12F675 has one GPIO six IO pins). Refer schematic circuit 
// diagram "06aut358 Issue C Mods.pdf".
//------------------------------------------------------------------------------
#define ANALOG_VOLT  (AN0)    // GP0/AN0/CIN+/ICSPDAT                   (J2-P11)
#define SET_BUTTON   (GPIO1)  // GP1/AN1/CIN-/VREF/ICSPCLK              (J2-P12)       
#define LED_INTERNAL (GPIO2)  // GP2/AN2/T0CLKI/INT/COUT
#define NOT_USED_3   (GPIO3)  // GP3/MCLR/Vpp                           (J1-P05)
#define OUTPUT_12V   (GPIO4)  // GP4/AN3/T1G/OSC2/CLKOUT      (*J1-P08, *J1-P09)
#define INPUT_PULSE  (GPIO5)  // GP5/T1CLKI/OSC1/CLKIN                  (J1-P02)
//------------------------------------------------------------------------------
// Configuration: to be used in main() to start up the device. Refer 
// PIC12F629/675 data sheet DS41190G chapter 3.0 GPIO PORT".
//------------------------------------------------------------------------------    
#define OPTION_WPU (0b01111111)         //---- MSb cleared to zero to enable WPU
#define TRISIO_CFG (0b00100011)         //------- '1' = input and high impedence
#define WPU_CONFIG (0b00100010)         // internal weak pull-up {5, 4, 2, 1, 0}
#define IOC_CONFIG (0b00100000)         //-- Interrupt on change {5, 4, 2, 1, 0}
#define T1CON_CONF (0b00000000)
    
#ifdef	__cplusplus
}
#endif

#endif	/* DEVICECONFIG_H */

//-------------------------------------------------------------------------- END
