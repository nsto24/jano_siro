/* 
 * File:                spi_functions.h   
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    31.08.2026 18:26 - Created file
 */

// Guard condition so that contents are not included more than once. 
#ifndef SPI_FUNCTIONS_H
#define	SPI_FUNCTIONS_H

// Slave specific read and write functions

void Init_SPI(void);

void SPI_Write_CH1(unsigned char data);

unsigned char SPI_Read_CH1(void);

void SPI_Write_CH2(unsigned char data);

unsigned char SPI_Read_CH2(void);

void SPI_Write_Outputs(unsigned int data);

unsigned int SPI_Read_Inputs(void);

void SPI_Write_Data_Display(unsigned char data);

void SPI_Write_Data_Array_Display(unsigned char* data, unsigned int length);

void SPI_Write_Command_Display(unsigned char cmd);

void SPI_Write_CanBus(unsigned char adress, unsigned char data);

unsigned char SPI_Read_CanBus(unsigned char adress);

void SPI_Reset_CanBus(void);

// Shift registers (ASCII art)
/*
INPUT SHIFT REGISTERS                      OUTPUT SHIFT REGISTERS

    [Furthest SR - High Byte]                 [Furthest SR - High Byte]
    External   Internal Latch                 Internal Shift   External 
      Pins         Buffer                          Buffer         Pins
      IN15 ------> [ 1 ]                           [ 1 ] -------> OUT15
      IN14 ------> [ 1 ]                           [ 0 ] -------> OUT14
      IN13 ------> [ 0 ]                           [ 1 ] -------> OUT13
      IN12 ------> [ 0 ]                           [ 0 ] -------> OUT12
      IN11 ------> [ 1 ]                           [ 0 ] -------> OUT11
      IN10 ------> [ 1 ]                           [ 1 ] -------> OUT10
      IN09 ------> [ 0 ]                           [ 0 ] -------> OUT09
      IN08 ------> [ 0 ]                           [ 1 ] -------> OUT08
                     |                               ^
                     | (Overflows to next SR)        | (Overflows from prior SR)
                     v                               |
      Closest SR - Low Byte]                   [Closest SR - Low Byte]
     External   Internal Latch                 Internal Shift   External 
      Pins         Buffer                          Buffer         Pins
      IN07 ------> [ 1 ]                           [ 0 ] -------> OUT07
      IN06 ------> [ 1 ]                           [ 0 ] -------> OUT06
      IN05 ------> [ 1 ]                           [ 1 ] -------> OUT05
      IN04 ------> [ 1 ]                           [ 1 ] -------> OUT04
      IN03 ------> [ 0 ]                           [ 1 ] -------> OUT03
      IN02 ------> [ 0 ]                           [ 1 ] -------> OUT02
      IN01 ------> [ 0 ]                           [ 0 ] -------> OUT01
      IN00 ------> [ 0 ]                           [ 0 ] -------> OUT00
                     |                               ^
                     | SDI                           | SDO 
=====================|===============================|======================
                     v                               |
            [ SPI Read Buffer ]             [ SPI Write Buffer ]
            Received 1st: 0xF0              Sent 1st: 0xA5 (High Byte)
            Received 2nd: 0xCC              Sent 2nd: 0x3C (Low Byte)

                  (Data combined: 0xCCF0)   (Data source: 0xA53C)
*/


#endif	/* SPI_FUNCTIONS_H */

