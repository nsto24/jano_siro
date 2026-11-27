/*
 * File:                main.c
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    31.08.2026 15:55 - Created file
 */

// #include commands
#include "globals.h"
#include "ssd1312.h"
#include "mode_machines.h"
#include "uart.h"
#include "spi_functions.h"
#include "hc05.h"
#include "dfplayermini.h"
#include "fonts.h"
#include "adc_functions.h"
#include "inputs.h"

#include <string.h>

volatile unsigned char UART_received_data1;
volatile unsigned char UART_new_received_message1 = 0;
volatile unsigned char UART_received_data2;
volatile unsigned char UART_new_received_message2 = 0;

volatile unsigned char FLAG_redraw_display = 0;

// Global variable definition
typedef enum states{
    stateInit=-1,                   // -1 for startup safe
    stateMenu,
    stateError
    /*Here all states: "stateXYZ"*/
} state_t;                          // Enums for state machine
static state_t currentState;        // To save current state
static state_t lastState;           // To save last state (edges)

// Implementation of state machine

static void transitionTo(state_t newState){
    lastState = currentState;
    currentState = newState;
}

static char wasTransition(void){
    return currentState != lastState;
}

static void updateLastState(void){
    lastState = currentState;
}

// Initialization routines

void Init_Pins(void){    
    // Analog/digital pin definition
    ANSELA = 0x0B;      // RA3, RA1 and RA0 as analoge
    ANSELB = 0x00;      // All Port B digital
    ANSELC = 0x00;      // All Port C digital
    ANSELD = 0x00;      // All Port D digital
    ANSELE = 0x00;      // All Port E digital
    
    TRISAbits.TRISA0 = 1; // RA0 defined as input for Joystick X
    TRISAbits.TRISA1 = 1; // RA1 defined as input for Joystick Y
    TRISAbits.TRISA2 = 1; // RA2 defined as input for Joystick button
    
    TRISBbits.TRISB0 = 0; // RB0 defined as output for display reset
    TRISBbits.TRISB4 = 0; // RB4 defined as output for display data/command
    TRISBbits.TRISB5 = 0; // RB5 defined as output for display chip select
    TRISBbits.TRISB6 = 0; // RB6 defined as output for inputs chip select snapshot
    
    // SPI
    TRISDbits.TRISD2 = 0; // RD2 defined as output for chip select output shift registers
    TRISDbits.TRISD5 = 0; // RD5 defined as output for chip select input shift registers
    
    TRISCbits.TRISC3 = 0; // RC3 defined as output for SPI CH1 clock
    TRISCbits.TRISC4 = 1; // RC4 defined as input for SPI CH1 data in
    TRISCbits.TRISC5 = 0; // RC5 defined as output for SPI CH1 data out
    TRISDbits.TRISD0 = 0; // RD0 defined as output for SPI CH2 clock
    TRISDbits.TRISD1 = 1; // RD1 defined as input for SPI CH2 data in
    TRISDbits.TRISD4 = 0; // RD4 defined as output for SPI CH2 data out
    
    // UART
    TRISCbits.TRISC0 = 1; // RC0 defined as input for bluetooth state 
    TRISCbits.TRISC1 = 0; // RC1 defined as output for bluetooth enable
    TRISCbits.TRISC6 = 1; // RC6 defined as input for UART CH1 transmit
    TRISCbits.TRISC7 = 1; // RC7 defined as input for UART CH1 receive
    
    TRISAbits.TRISA3 = 1; // RA3 defined as input for volume potentiometer
    TRISCbits.TRISC2 = 1; // RC2 defined as input for DFPlayer busy
    TRISBbits.TRISB3 = 0; // RB3 defined as output for Speaker mute relais
    TRISDbits.TRISD6 = 1; // RD6 defined as input for UART CH2 transmit
    TRISDbits.TRISD7 = 1; // RD7 defined as input for UART CH2 receive
}

void Init_ADC(void){
    ADCON1 = ADCON1_def;
    ADCON2 = ADCON2_def;
    Adc_Enable();
}

void Init_Timers(void){
    T0CON = T0CON_def;
    TMR0H = TMR0H_VAL;   // Load pre-calculated high byte for 10ms
    TMR0L = TMR0L_VAL;   // Load pre-calculated low byte for 10ms
}

void Init_Interrupts(void){
    // SPI interrupt initialization
    SSP1IF = 0;     // Clear SPI CH1 interrupt flag
    SSP2IF = 0;     // Clear SPI CH2 interrupt flag
    
    SSP1IE = 0;     // Disable SPI CH1 interrupt
    SSP2IE = 0;     // Disable SPI CH2 interrupt
    
    // UART enable receive interrupts
    RC1IE = 1;      // Enable UART CH1 receive interrupt
    RC2IE = 1;      // Enable UART CH2 receive interrupt
    
    // Timer0 enable
    TMR0IF = 0;     // Clear interrupt flag
    TMR0IE = 1;     // Enable Timer0 interrupt
    
    // Global and Peripheral Interrupt initialization
    PEIE = 1;       // Peripheral Interrupt Enable
    GIE = 1;        // Global Interrupt Enable
}

// Interrupt serice routines

void __interrupt() Interrupts(void){    
    // Call Button Reading ISR Handler (reading buttons and analysing changes)
    Buttons_ISR_Handler();
    
    // Call UART ISR Handler (check if message was received and handle message)
    UART_ISR_Handler();
}

void main(void) {
    // Local variable definition
    unsigned char firstCycleFlag = 1;
    unsigned char buf[20] = {0};
    
    // Calling of Init Functions
    Init_Pins();
    
    __delay_ms(2000);
    
    DISPLAY_RES = 1;
    SPEAKER_MUTE = 0;
    
    Init_ADC();
    Init_Timers();
    Init_SPI();
    SPI_Write_Outputs(0x0000);
    Init_UART();
    Init_Display();
    //Init_HC05();
    Init_Interrupts();
    Init_DFP();  

    while(1){               
        // Pre cycle functions
        Sync_Inputs();
        // HC05_SyncState();
        DFP_UpdateState();
        
        if(hasVolumeChanged()){DFP_specificVolume(getVolumeState());}
        
        state_machine_menu();
        
        // After cycle functions
        
        if(FLAG_redraw_display){
            Display_Update();
        }
        
        __delay_ms(10);       
    }
    
    return;
}
