/* 
 * File:                globals.h
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    31.08.2026 15:51 - Created file
 */

// Guard condition so that contents are not included more than once.  
#ifndef GLOBALS_H
#define	GLOBALS_H

// Load first mcu config
#include "mcu_config.h"

// Standard #include
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

// Custom constants
#define BUTTON_COUNT    12      // Number of pressable buttons on "screen"

#define TX_BUFFER_SIZE  64      // Size of buffer for received string UART messages
#define SPI_BUFFER_SIZE 64      // Size of buffer for received string SPI messages

// Timer 0 calculation
#define T0PS            8           // Timer0 Prescaler (look T0CON_def)
#define TARGET_MS       10          // Target interrupt time in milliseconds
#define TIMER0_TICKS    (((_XTAL_FREQ / 4000) * TARGET_MS) / T0PS)
#define TIMER0_RELOAD   (65536 - TIMER0_TICKS)
#define TMR0H_VAL       (unsigned char)(TIMER0_RELOAD >> 8)
#define TMR0L_VAL       (unsigned char)(TIMER0_RELOAD & 0xFF)

// Global variables for all modules

extern volatile unsigned char UART_received_data1;
extern volatile unsigned char UART_new_received_message1;
extern volatile unsigned char UART_received_data2;
extern volatile unsigned char UART_new_received_message2;

#endif	/* GLOBALS_H */

