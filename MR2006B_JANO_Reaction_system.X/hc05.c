/*
 * File:                hc05.c
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    06.10.2026, 19:18
 */

// #include
#include "globals.h"
#include "uart.h"
#include <string.h>

#define HC05_TARGET_BT_ADRESS_1     0x582A
#define HC05_TARGET_BT_ADRESS_2     0xBD
#define HC05_TARGET_BT_ADRESS_3     0x74434A

#define UART_BAUD       9600UL
// BRG16 = 1, BRGH = 1  ->  n = Fosc / (4 * Baud) - 1, rounded
#define UART_BRG_VAL    (((_XTAL_FREQ + 2UL * UART_BAUD) / (4UL * UART_BAUD)) - 1)   // = 416 (0x01A0)
// Pre-calculate the 38400 baud rate value using the 16-bit High-Speed formula (Divisor = 4)
#define UART_BAUD_AT         38400UL
#define UART_BRG_AT_VAL      (((_XTAL_FREQ + 2UL * UART_BAUD_AT) / (4UL * UART_BAUD_AT)) - 1)

void Init_HC05(void){
    char bind_cmd[40] = "AT+BIND=";
    char target_adr[30];
    
    // Form address string and combine with command prefix
    sprintf(target_adr, "%X,%X,%lX\r\n", HC05_TARGET_BT_ADRESS_1, HC05_TARGET_BT_ADRESS_2, HC05_TARGET_BT_ADRESS_3);
    strcat(bind_cmd, target_adr);
    
    // Pull KEY pin to high
    HC05_ENABLE_KEY = 1;
    
    // ---------------------------------------------------------
    // Reconfigure baud rate to 38400 (Writing both High and Low bytes)
    // ---------------------------------------------------------
    SPBRGH1 = (unsigned char)(UART_BRG_AT_VAL >> 8);
    SPBRG1  = (unsigned char)(UART_BRG_AT_VAL);
    
    // Send AT commands to HC-05 module
    UART_WriteString1("AT\r\n");                    // Initializing AT-Command transfer
    __delay_ms(200);
    UART_WriteString1("AT+ROLE=1\r\n");             // Set module as Master
    __delay_ms(200);
    UART_WriteString1("AT+CMODE=0\r\n");            // Connect to fixed bluetooth device!
    __delay_ms(200);
    UART_WriteString1(bind_cmd);                    // Setting BT adress of slave
    __delay_ms(200);
    UART_WriteString1("AT+UART=9600,0,0\r\n");      // Setting baud rate of later data transfers to 9600
    __delay_ms(200);
    UART_WriteString1("AT+RESET\r\n");              // Reset and exit AT mode
    __delay_ms(500);
    
    // Pull KEY pin back to ground
    HC05_ENABLE_KEY = 0;
    
    // ---------------------------------------------------------
    // Restore baud rate of 9600 (Writing both High and Low bytes)
    // ---------------------------------------------------------
    SPBRGH1 = (unsigned char)(UART_BRG_VAL >> 8);   // Assuming UART_BRG_VAL is available globally or redefined here
    SPBRG1  = (unsigned char)(UART_BRG_VAL);
}