/* 
 * File:                mcu_config.h 
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    31.08.2026 15:51 - Created file
 */

// Guard condition so that contents are not included more than once.  
#ifndef MCU_CONFIG_H
#define	MCU_CONFIG_H

#include <xc.h>

// PIC18F46K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = HSHP      // Oscillator Selection bits (EC oscillator (high power, >16 MHz))
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock is always enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)



// MCU #define

#define _XTAL_FREQ          16000000    // 16-MHz crystal frequency

#define _ADC_RES_BITS       10          // ADC with 10 bit resolution

// PORT pins definition
#define JOYSTICK_X          PORTAbits.RA0       // Joystick position X pin (Input)
#define JOYSTICK_Y          PORTAbits.RA1       // Joystick position Y pin (Input)
#define JOYSTICK_SW         PORTAbits.RA2       // Joystick switch pressed pin (Input)

#define DISPLAY_DC          LATBbits.LATB4      // Display data/command pin (Output)

#define CANBUS_MSG_INT      PORTBbits.RB0       // Canbus module interrupt (Input)

// SPI Channel definitions
#define SPI_CS_DISPLAY      LATBbits.LATB5      // Chip select for display (Output)
#define SPI_CS_OUTPUTS      LATDbits.LATD2      // Chip select for output shift registers (Output)
#define SPI_CS_CANBUS       LATDbits.LATD3      // Chip select for canbus module (Output)
#define SPI_CS_INPUTS       LATDbits.LATD5      // Chip select for input shift registers (Output)
#define SPI_CS_INPUTS_SNAP  LATBbits.LATB6      // Chip select for intut shift registers snapshot (Output)

#define SPI_SCK1            LATCbits.LATC3      // SPI channel 1 clock
#define SPI_SDI1            PORTCbits.RC4       // SPI channel 1 data in
#define SPI_SDO1            LATCbits.LATC5      // SPI channel 1 data out
#define SPI_SCK2            LATDbits.LATD0      // SPI channel 2 clock
#define SPI_SDI2            PORTDbits.RD1       // SPI channel 2 data in
#define SPI_SDO2            LATDbits.LATD4      // SPI channel 2 data out

// UART definitions
#define HC05_STATE          PORTCbits.RC0       // Bluetooth module state pin (Input)
#define HC05_ENABLE_KEY     LATCbits.LATC1      // Bluetooth module enable pin (Output)
#define HC05_UART_TX        LATCbits.LATC6      // UART channel 1 transmit
#define HC05_UART_RX        PORTCbits.RC7       // UART channel 1 receive

#define DFP_BUSY            PORTCbits.RC2       // DFPlayer busy (Input)
#define SPEAKER_MUTE        LATBbits.LATB3      // Speaker mute relais (Output)
#define DFP_UART_TX         LATDbits.LATD6      // UART channel 2 transmit
#define DFP_UART_RX         PORTDbits.RD7       // UART channel 2 receive

// ADC Channel definitions
#define ADC_CH_JOYSTICK_X   0x00000     // Channel number Joystick x axis channel
#define ADC_CH_JOYSTICK_Y   0x00001     // Channel number Joystick y axis channel

// ADC Configuration
// ADCON0 gets reconfigured in runtime (channel selection, start of conversion)
// for SPI the Pins AN12 to AN10 must be declared as digital
#define ADCON1_def          0b00000001  // Voltage definitions
                                        // 7    :   1       Special trigger bit of CTMU is selected
                                        // 6-4  :   000     Unused
                                        // 3-2  :   00      Positive Voltage Reference set to internal Vdd
                                        // 1-0  :   00      Negative Voltage Reference set to internal Vss
#define ADCON2_def          0b10100010  // Clock definition, acquisition time
                                        // 7    :   1       result right justified
                                        // 6    :   0       unused
                                        // 5-3  :   100     A/D Acquisition time select bits (8 Tad)
                                        // 2-0  :   010     A/D Conversion clock select bits (Fosc/32)

// SPI Configuration
#define SSPSTAT_def         0b01000000  // Definition MSSP status register
                                        // 7   :  0     Input data sampled at middle of data output time
                                        // 6   :  1     Transmit occurs on transition from active to Idle clock state
                                        // 5-1 :  0     Used in I2C only (irrelevant)
                                        // 0   :  0     Receive not complete, SSPBUF is empty (Buffer Full Status Bit - BF)
#define SSPCON1_def         0b00100001  // Definition MSSP control register 1
                                        // 7   :  0     No collision (Write Collision Detection Bit - WCOL)
                                        // 6   :  0     No overflow (Receive Overflow Indicator Bit - SSPOV)
                                        // 5   :  1     Enables serial port and configures SCK, SDO, SDI and SS as serial port pins
                                        // 4   :  0     Idle state for clock is a LOW level
                                        // 3-0 :  0001  SPI Master mode, clock = Fosc/16 (1 MHz clock)

// Interrupt Configuration
#define INTCON_def          0b11000000  // Definition of INTCON control register
                                        // 7   :  1     All global interrupts are enabled   
                                        // 6   :  1     All peripheral interrupts are enabled
                                        // 5   :  0     Overflow interrupt is disabled
                                        // 4   :  ?     External interrupt is disabled CAUTION 
                                        // 3   :  0     Port change interrupt is disabled
                                        // 2   :  0     Overflow interrupt flag is low (has not accured)
                                        // 1   :  0     External interrupt flag is low (has not accured)
                                        // 0   :  0     Change interrupt flag is low (has not accured)
#define INTCON2_def         0b10000000  // Definition of INTCON2 control register
                                        // 7   :  1     Port B pull ups are disabled
                                        // 6   :  ?     External interrupt 0 is set to ? edge
                                        // 5   :  ?     External interrupt 1 is set to ? edge
                                        // 4   :  ?     External interrupt 2 is set to ? edge
                                        // 3   :  0     Unimplemented
                                        // 2   :  0     Overflow interrupt priority set to low
                                        // 1   :  0     Unimplemented
                                        // 0   :  0     Port change interrupt priority set to low
#define INTCON3_def         0b

// EUSART Configuration

#define TXSTA_def           0b00100100  // Definition of TXSTA control register
                                        // 7   :  0     Clock source (dont care since asychron is used)
                                        // 6   :  0     8 bit transmission
                                        // 5   :  1     Transimit enabled
                                        // 4   :  0     Asychronous mode
                                        // 3   :  0     Sync break transmission completed
                                        // 2   :  1     High speed baud rate selected
                                        // 1   :  0     Transmit register status (full)
                                        // 0   :  0     9th bit of transmission data (not used)

#define RCSTA_def           0b10010000  // Definition of RCSTA control register
                                        // 7   :  1     Serial port is enabled
                                        // 6   :  0     8 bit reception is selected
                                        // 5   :  0     dont care
                                        // 4   :  1     Receiver enabled
                                        // 3   :  0     dont care
                                        // 2   :  0     No framing error
                                        // 1   :  0     No overrun error
                                        // 0   :  0     9th bit of received data (not used)

#endif	/* MCU_CONFIG_H */