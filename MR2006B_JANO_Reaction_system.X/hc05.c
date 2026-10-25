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

#define HC05_TARGET_BT_ADRESS_1     0x0e4a 
#define HC05_TARGET_BT_ADRESS_2     0x35
#define HC05_TARGET_BT_ADRESS_3     0xdc7af5

void Init_HC05(void){
    /* Process of initialization
     * 
     * 1. Pull "KEY" pin of HC-05 to high to enter AT command mode
     * 2. Reinitialize Baud rate of UART port to 38400 baud
     * 3. Send AT commands to initialize module as master
     * 4. Drop "KEY" pin of HC-05 back to low and reconfigure baud rate to standard
     */
    // Variable definition
    char bind_cmd[40] = "AT+BIND=";
    char target_adr[30];
    
    // Form address string and combine with command prefix
    sprintf(target_adr, "%X,%X,%lX\r\n", HC05_TARGET_BT_ADRESS_1, HC05_TARGET_BT_ADRESS_2, HC05_TARGET_BT_ADRESS_3);
    strcat(bind_cmd, target_adr);
    
    // Pull KEY pin to high
    HC05_ENABLE_KEY = 1;
    
    // Reconfigure baud rate to 38400
    SPBRG = (_XTAL_FREQ / (16UL*38400UL)) - 1;     // 38400 baud at 8MHz
    
    // Send AT commands to HC-05 module
    UART_WriteString1("AT\r\n");                    // Initializing AT-Command transfer
    __delay_ms(200);
    UART_WriteString1("AT+ROLE=1\r\n");             // Set module as Master
    __delay_ms(200);
    UART_WriteString1("AT+CMODE=0\r\n");            // Connect to fixed bluetooth device!
    __delay_ms(200);
    UART_WriteString1(bind_cmd);                    // Setting BT adress of slave
    __delay_ms(200);
    UART_WriteString1("AT+UART=9600,0,0\r\n");      // Setting baud rate of later data transfers to 9600 (standard UART speed)
    __delay_ms(200);
    UART_WriteString1("AT+RESET\r\n");              // Reset and exit AT mode
    __delay_ms(500);
    
    // Pull KEY pin back to ground
    HC05_ENABLE_KEY = 0;
    // Restore baud rate of 9600
    SPBRG = (_XTAL_FREQ / (16UL*9600UL)) - 1;
}