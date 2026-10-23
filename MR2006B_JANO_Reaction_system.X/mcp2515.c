/* 
 * File:                mcp2515.c   
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    05.10.2026 15:39 - Created file
 */

// #include
#include "globals.h"
#include "mcp2515_commands.h"
#include "spi_functions.h"

void Init_MCP(void){
    MCP_Setup();
}

void MCP_Setup(void){
    // Reset MCP Module
    SPI_Reset_CanBus();
    
    // Configure bit timing
    SPI_Write_CanBus(MCP_REGISTER_CNF1, 0x00);      // 00000000
                                                    // 7-6  : 00    Synchronization Jump Width Length set to 1 T_Q
                                                    // 5-0  : 00000 Baud Rate Prescaler set to T_Q = 2 x 1/F_OSC
    
    SPI_Write_CanBus(MCP_REGISTER_CNF2, 0x90);      // 10010000
                                                    // 7    : 1     PS2 Bit Time Length (determined by PHSEG2[2:0] bits of CNF3)
                                                    // 6    : 0     Bus line is sampled once at the sample point
                                                    // 5-3  : 010   PS1 Length (PHSEG1[2:0] + 1) x T_Q
                                                    // 2-0  : 000   Propagation Segment Length (PRSEG[2:0] + 1) x T_Q
    
    SPI_Write_CanBus(MCP_REGISTER_CNF3, 0x82);      // 10000010
                                                    // 7    : 1     CLKOUT pin is enabled for SOF signal
                                                    // 6    : 0     Wake-up filter is disabled
                                                    // 5-3  : 000   Unimplemented
                                                    // 2-0  : 010   PS2 Length (PHSEG2[2:0] + 1) x T_Q
    
    // Configure interrupt
    SPI_Write_CanBus(MCP_REGISTER_CANINTE, 0x01);   // 00000001
                                                    // 7-1  : 0-0   ALl kinds of interrupts disabled
                                                    // 0    : 1     Interrupt when message was received in RXB0
    
    // Switch to normal operation mode
    SPI_Write_CanBus(MCP_REGISTER_CANCTRL, 0x00);   // 000?00??
                                                    // 7-5  : 000   Sets normal operation mode
                                                    // 4    : ?     
                                                    // 3    : 0     Disables One-Shot-Mode: messages will reattempt transmission
                                                    // 2    : 0     CLKOUT pin is disabled
                                                    // 1-0  : ??
}

void MCP_TransmitMessage(unsigned int standard_id, unsigned char *data, unsigned char length){
    /*  Structure of CAN Message (for coding purposes):
     *  11 bit identifier:          Destination identifier of message (0x123 is standardly used)
     *  4 bit data length code:     Defines length of the following data field in message (1 to 8 bytes)
     *  8x8 bit data field:         8 bytes of data for the receiving device
     * 
     *  Further the MCP2515 implements an automatic address pointer incrementer, moving the pointer by one byte after receiving byte
     * 
     *  Register order of transmit buffer 0 (written to in display order):
     *  TXB0SIDH    (Transmit Tx Buffer register 0 standard identifier high byte)
     *  TXB0SIDL    (Transmit Tx Buffer register 0 standard identifier low byte)
     *  TXB0EID8    (Transmit Tx Buffer register 0 extended identifier high byte)
     *  TXB0EID0    (Transmit Tx Buffer register 0 extended identifier low byte)
     *  TXB0DLC     (Transmit Tx Buffer register 0 data length code)
     *  TXB0D0      (Transmit Tx Buffer register 0 data byte 0)
     *  ...         
     *  TXB0D7      (Transmit Tx Buffer register 0 data byte 7)
     */
    
    // standard_id is usually 0x123
    unsigned int i = 0;
  
    unsigned char standard_id_h = (unsigned char)(standard_id >> 3);             // [10:3] -> [7:0]
    unsigned char standard_id_l = (unsigned char)((standard_id & 0x07) << 5);    // [2:0] -> [7:5]
    
    // Insure the length of the data is maximum of 8 bytes
    if(length > 8){length = 8;}
    
    // Select MCP module
    SPI_CS_CANBUS = 0;
    __delay_us(20);
    
    // Send Instruction: LOAD TX BUFFER 0 beginning at SIDH
    SPI_Write_CH1(LOAD_TX_BUFFER_0SIDH);
    
    // Load standard identifier: 
    SPI_Write_CH1(standard_id_h);   // TXB0SIDH is filled with high byte of standard id (Transmit Tx Buffer register 0 standard identifier high byte)
    SPI_Write_CH1(standard_id_l);   // TXB0SIDL is filled with low byte of standard id (Transmit Tx Buffer register 0 standard identifier low byte)
    
    // Fill unused identifier bytes with 0
    SPI_Write_CH1(0x00);    // TXB0EID8 is filled with 0x00 (Transmit Tx Buffer register 0 extended identifier high byte)
    SPI_Write_CH1(0x00);    // TXB0EID0 is filled with 0x00 (Transmit Tx Buffer register 0 extended identifier low byte)
    
    // Write length of following data into TXB0DLC register
    SPI_Write_CH1(length & 0x0F);   // TXB0DLC is filled with length (Transmit Tx Buffer register 0 data length code)
    
    // Serial write of data into the following data registers TXB0D0 ... TXB0D7 (Transmit Tx Buffer register 0 data byte 0-7)(depended of length)
    for(i = 0; i < length; i++){
        SPI_Write_CH1(data[i]);
    }
    
    // End buffer load
    SPI_CS_CANBUS = 1;
    
    // Triggering of transmission with RTS command
    __delay_us(10);     // Settling delay
    
    SPI_CS_CANBUS = 0;
    __delay_us(20);
    
    // Instruction: Request-to-send (RTS) for Buffer 0
    SPI_Write_CH1(MESSAGE_RTS_TXB0);
    
    // End transmission by deselecting MCP module
    SPI_CS_CANBUS = 1;
}

unsigned char MCP_ReceiveMessage(unsigned int *standard_id, unsigned char *data, unsigned char *length){
    /*  Structure of CAN Message (for coding purposes):
     *  11 bit identifier:          Destination identifier of message (0x123 is standardly used)
     *  4 bit data length code:     Defines length of the following data field in message (1 to 8 bytes)
     *  8x8 bit data field:         8 bytes of data for the receiving device
     *
     *  Further the MCP2515 implements an automatic address pointer incrementer, moving the pointer by one byte after receiving byte
     * 
     *  Register order of receive buffer 0 (read in the display order):
     *  RXB0SIDH    (Read Rx Buffer Register 0 standard identifier high byte)
     *  RXB0SIDL    (Read Rx Buffer Register 0 standard identifier low byte)
     *  RXB0EID8    (Read Rx Buffer Register 0 extended identifier high byte)
     *  RXB0EID0    (Read Rx Buffer Register 0 extended identifier low byte)
     *  RXB0DLC     (Read Rx Buffer Register 0 data length code)
     *  RXB0D0      (Read Rx Buffer Register 0 data byte 0)
     *  ...
     *  RXB0D7      (Read Rx Buffer Register 0 data byte 7)
     */
    
    unsigned char standard_id_h = 0;
    unsigned char standard_id_l = 0;
    unsigned char data_length_code = 0;
    unsigned char i = 0;
    
    
    // Select MCP module
    SPI_CS_CANBUS = 0;
    __delay_us(20);
    
    // Instruction: READ RX BUFFER 0 starting at SIDH (Read Rx Buffer Register 0 standard identifier high byte)
    SPI_Write_CH1(READ_RX_BUFFER_0SIDH);
    
    // Read identifier bytes
    standard_id_h = SPI_Read_CH1();             // RXB0SIDH (Read Rx Buffer Register 0 standard identifier high byte)
    standard_id_l = SPI_Read_CH1();             // RXB0SIDL (Read Rx Buffer Register 0 standard identifier low byte)
    
    // Discard unused identifier bits
    SPI_Read_CH1();                             // RXB0EID8 (Read Rx Buffer Register 0 extended identifier high byte)
    SPI_Read_CH1();                             // RXB0EID0 (Read Rx Buffer Register 0 extended identifier low byte)
    
    // Read data length and write to variable
    data_length_code = SPI_Read_CH1() & 0x0F;   // RXB0DLC (Read Rx Buffer Register 0 data length code) and mask 4 bit length code
    *length = data_length_code;             // Write 4 bit length code to length variable
    
    // Reconstruct 11 bit identifier and write to variable
    *standard_id = ((unsigned int)standard_id_h << 3) | (standard_id_l >> 5);   // High byte: [7:0] -> [10:3]
                                                                                // Low byte:  [7:5] -> [2:0]
    
    // Serial read data into array from RXB0D0 to RXB0D7 (Read Rx Buffer Register 0 data byte 0-7)(depended on length)
    for(i = 0; i < data_length_code && i < 8; i++){
        data[i] = SPI_Read_CH1();
    }
    
    // End read transaction
    SPI_CS_CANBUS = 1;
    
    return 1;   // Message succesfully unpacked
}

void MCP_ClearInterruptFlag(void){
    // Select MCP module
    SPI_CS_CANBUS = 0;
    __delay_us(20);
    
    // Instruction: BIT_MODIFY
    SPI_Write_CH1(BIT_MODIFY);
    
    // Send adress of target register
    SPI_Write_CH1(MCP_REGISTER_CANINTF);
    
    // Send mask byte
    SPI_Write_CH1(0x01);    // Only bit 0 is "selected" (Bit 0: Receive Buffer 0 Full Interrupt Flag -> Buffer 0 is full)
    
    // Send data byte
    SPI_Write_CH1(0x00);    // Bit 0 is now cleared
    
    // End clear transaction
    SPI_CS_CANBUS = 1;
}

void MCP_ISR_Handler(void){
    if(INT0IF){                          // CAUTION!!! Not real flag, see wiring
        // Read received message
        if(MCP_ReceiveMessage(&CAN_received_id, CAN_received_data, & CAN_received_length)){
            CAN_new_received_message = 1;
        }
        // Clear interrupt flag inside MCP
        MCP_ClearInterruptFlag();
        // Clear interrupt flag inside PIC
        INT0IF = 0;
    }
}