/* 
 * File:                uart.h   
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    05.10.2026 19:02 - Created file
 */

#ifndef UART_H
#define	UART_H

unsigned char UART_TryWriteRegister1(unsigned char data);

unsigned char UART_TryWriteRegister2(unsigned char data);

unsigned char UART_Read1(void);

unsigned char UART_Read2(void);

void UART_WriteChar1(unsigned char data);

void UART_WriteChar2(unsigned char data);

void UART_WriteString1(const char *str);

void UART_WriteString2(const char *str);

void UART_ISR_Handler(void);

void Init_UART(void);

#endif	/* UART_H */

