/* 
 * File:                spi_functions.c   
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    31.08.2026 18:54 - Created file
 */

// #include
#include "globals.h"
#include "spi_functions.h"

// Initialization function
void Init_SPI(void){
    SSP1STAT = SSPSTAT_def;
    SSP2STAT = SSPSTAT_def;
    
    SSP1CON1 = SSPCON1_def;
    SSP2CON1 = SSPCON2_def;
    
    SSP1IF = 0;       // Clear SSP1IF flag (no data received)
    SSP2IF = 0;       // Clear SSP1IF flag (no data received)
    
    // Deselecting all slaves
    SPI_CS_DISPLAY = 1;
    SPI_CS_OUTPUTS = 1;
    SPI_CS_INPUTS = 1;
    SPI_CS_INPUTS_SNAP = 0;
}

// SPI elementary write and read functions

void SPI_Write_CH1(unsigned char data){
    unsigned char data_flush;
    SSP1BUF = data;             // Copy data in SSP1BUF to transmit
    while(!SSP1IF);             // Wait for complete 1 byte transmission 
    SSP1IF = 0;                 // Clear SSP1IF flag 
    data_flush = SSP1BUF;       // Flush the data (clear SSP1BUF register)
}

unsigned char SPI_Read_CH1(){    
    SSP1BUF = 0xff;             // Copy flush byte in SSP1BUF
    while(!SSP1IF);             // Wait for complete 1 byte transmission
    SSP1IF = 0;                 // Clear SSP1IF flag 
    return(SSP1BUF);            // Return received byte   
}

void SPI_Write_CH2(unsigned char data){
    unsigned char data_flush;
    SSP2BUF = data;             // Copy data in SSP2BUF to transmit
    while(!SSP2IF);             // Wait for complete 1 byte transmission 
    SSP2IF = 0;                 // Clear SSP2IF flag 
    data_flush = SSP2BUF;       // Flush the data (clear SSP2BUF register)
}

unsigned char SPI_Read_CH2(){    
    SSP2BUF = 0xff;             // Copy flush byte in SSP2BUF
    while(!SSP2IF);             // Wait for complete 1 byte transmission
    SSP2IF = 0;                 // Clear SSP2IF flag 
    return(SSP2BUF);            // Return received byte   
}

// SPI slave specific write and read functions

void SPI_Write_Outputs(unsigned int data){
    // High byte is written first (shift register further from PIC)
    // Low byte is written second (shift register closer to PIC)
    SPI_CS_OUTPUTS = 0;                     // Pull pin of outputs to low (active slave)
    __delay_us(20);
    SPI_Write_CH1((data >> 8) & 0x00FF);    // Writing high byte to slave
    SPI_Write_CH1(data & 0x00FF);           // Writing low byte to slave
    SPI_CS_OUTPUTS = 1;                     // Pull pin of outputs to high (deactive slave)
}

unsigned int SPI_Read_Inputs(void){
    // Low byte is read first (shift register closer to PIC)
    // High byte is read second (shift register further from PIC)
    
    unsigned char low_byte = 0;
    unsigned char high_byte = 0;
    SPI_CS_INPUTS_SNAP = 1;                 // Pull pin of inputs to low (snapshot of inputs)
    __delay_us(20);
    SPI_CS_INPUTS_SNAP = 0;                 // Pull pin of inputs to high
    __delay_us(20);
    
    SPI_CS_INPUTS = 0;                      // Pull pin of inputs to low (snapshot of inputs)
    __delay_us(20);
    SPI_CS_INPUTS = 1;                      // Pull pin of inputs to high
    __delay_us(20);
    low_byte = SPI_Read_CH2();              // Reading low byte from slave
    high_byte = SPI_Read_CH2();             // Reading high byte from slave
    return (low_byte | ((unsigned int)high_byte << 8));   // Return read data
}

void SPI_Write_Command_Display(unsigned char cmd){
    DISPLAY_DC = 0;                         // Set display to "read command" mode
    SPI_CS_DISPLAY = 0;                     // Pull pin of inputs to low (active slave)
    __delay_us(20);
    SPI_Write_CH1(cmd);                     // Write command on SPI Bus
    SPI_CS_DISPLAY = 1;                     // Pull pin of inputs to high (deactive slave)
}

void SPI_Write_Data_Display(unsigned char data){
    DISPLAY_DC = 1;                         // Set display to "read data" mode
    SPI_CS_DISPLAY = 0;                     // Pull pin of inputs to low (active slave)
    __delay_us(20);
    SPI_Write_CH1(data);                    // Write command on SPI Bus
    SPI_CS_DISPLAY = 1;                     // Pull pin of inputs to high (deactive slave)
}

void SPI_Write_Data_Array_Display(unsigned char* data, unsigned int length){
    DISPLAY_DC = 1;                         // Set display to "read data" mode
    SPI_CS_DISPLAY = 0;                     // Pull pin of inputs to low (active slave)
    __delay_us(20);
    for(unsigned int i = 0; i < length; i++){
        SPI_Write_CH1(data[i]);             // Write command on SPI Bus
    }
    SPI_CS_DISPLAY = 1;                     // Pull pin of inputs to high (deactive slave)
}
