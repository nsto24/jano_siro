/* 
 * File:                uart.c   
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    05.10.2026 18:50 - Created file
 */

#include "globals.h"

/* ==========================================================================
 * RING BUFFER LAYOUT (Size 64 Bytes)
 * ==========================================================================
 * 
 *            Index:   0    1    2    3    ...  62   63
 *            Array: [ 'H', 'e', 'l', 'l', ...   \0 , 0  ]
 *                     ^              ^
 *                     |              |
 *                  TAIL           HEAD
 *               (Next out)     (Next in)
 *
 *  - EMPTY: HEAD == TAIL
 *  - FULL:  (HEAD + 1) % SIZE == TAIL
 *  - DATA FLOW: HEAD moves forward when writing; 
 *               TAIL moves forward when the ISR transmits a byte.
 * ========================================================================== */

volatile static unsigned char UART_tx_buffer1[TX_BUFFER_SIZE];  // Ring buffer to store to be transmitted characters
volatile static unsigned char UART_tx_head1 = 0;                // Next free slot in ring buffer a character can be written to
volatile static unsigned char UART_tx_tail1 = 0;                // Last character in ring buffer to be written next

volatile static unsigned char UART_tx_buffer2[TX_BUFFER_SIZE];  // Ring buffer to store to be transmitted characters
volatile static unsigned char UART_tx_head2 = 0;                // Next free slot in ring buffer a character can be written to
volatile static unsigned char UART_tx_tail2 = 0;                // Last character in ring buffer to be written next

void Init_UART(void){
    // Load definition into registers
    TXSTA1 = TXSTA_def;
    TXSTA2 = TXSTA_def;
    RCSTA1 = RCSTA_def;
    RCSTA2 = RCSTA_def;
    
    // Set baud rate generator (9600 baud)
    int baudSet = (_XTAL_FREQ / (16*9600)) - 1;
    SPBRG1 = baudSet;  
    SPBRG2 = baudSet;
}

unsigned char UART_TryWriteRegister1(unsigned char data){
    if(TX1IF != 0){      // Write data into transmission register TXREG (empty if TX1IF != 0) and return success
        TXREG1 = data;
        return 1;
    }
    else{               // Register still full, no data can be written
        return 0;
    }
}

unsigned char UART_TryWriteRegister2(unsigned char data){
    if(TX2IF != 0){      // Write data into transmission register TXREG (empty if TX2IF != 0) and return success
        TXREG2 = data;
        return 1;
    }
    else{               // Register still full, no data can be written
        return 0;
    }
}

unsigned char UART_Read1(void){
    // Return register of received data RCREG (RCIF interrupt flag is clear automatically)
    return RCREG1;
}

unsigned char UART_Read2(void){
    // Return register of received data RCREG (RCIF interrupt flag is clear automatically)
    return RCREG2;
}


void UART_WriteChar1(unsigned char data){
    unsigned char next_head = (UART_tx_head1 + 1) % TX_BUFFER_SIZE;  // Next place in ring buffer
    
    while(next_head == UART_tx_tail1);       // Wait if ring buffer is full
    
    // Add data to ring buffer
    UART_tx_buffer1[UART_tx_head1] = data;    // Add new character to ring buffer at position UART_tx_head
    UART_tx_head1 = next_head;               // Advance writing address to next slot in ring buffer (empty)
    
    // Enable EUSART transmit interrupt
    TX1IE = 1;
}

void UART_WriteChar2(unsigned char data){
    unsigned char next_head = (UART_tx_head2 + 1) % TX_BUFFER_SIZE;  // Next place in ring buffer
    
    while(next_head == UART_tx_tail2);       // Wait if ring buffer is full
    
    // Add data to ring buffer
    UART_tx_buffer2[UART_tx_head2] = data;    // Add new character to ring buffer at position UART_tx_head
    UART_tx_head2 = next_head;               // Advance writing address to next slot in ring buffer (empty)
    
    // Enable EUSART transmit interrupt
    TX2IE = 1;
}

void UART_WriteString1(const char *str){
    while(*str){
        UART_WriteChar1(*str);
        str++;
    }
}

void UART_WriteString2(const char *str){
    while(*str){
        UART_WriteChar2(*str);
        str++;
    }
}

void UART_ISR_Handler(void){
    if (TX1IF && TX1IE) {     // Interrupt enabled (TX1IE) and transmit register is empty (TX1IF) interrupt called
        if (UART_tx_head1 != UART_tx_tail1) {     // Buffer is not empty: pull next byte and send
            TXREG1 = UART_tx_buffer1[UART_tx_tail1];
            UART_tx_tail1 = (UART_tx_tail1 + 1) % TX_BUFFER_SIZE;
        } else {        // Char buffer is empty: disable TX interrupt
            TX1IE = 0;   // Stop interrupt when queue is empty
        }
    }
    
    if (TX2IF && TX2IE) {     // Interrupt enabled (TX1IE) and transmit register is empty (TX1IF) interrupt called
        if (UART_tx_head2 != UART_tx_tail2) {     // Buffer is not empty: pull next byte and send
            TXREG2 = UART_tx_buffer2[UART_tx_tail2];
            UART_tx_tail2 = (UART_tx_tail2 + 1) % TX_BUFFER_SIZE;
        } else {        // Char buffer is empty: disable TX interrupt
            TX2IE = 0;   // Stop interrupt when queue is empty
        }
    }
    
    if(RC1IF){
        UART_received_data1 = UART_Read1();
        UART_new_received_message1 = 1;
    }
    if(RC2IF){                       
        UART_received_data2 = UART_Read2();
        UART_new_received_message2 = 1;
    }
}