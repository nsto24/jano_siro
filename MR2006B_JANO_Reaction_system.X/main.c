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
#include "mcp2515.h"
#include "uart.h"
#include "spi_functions.h"
#include "hc05.h"

// Global variable instantiations
volatile unsigned char CAN_received_data[8];
volatile unsigned int  CAN_received_id;
volatile unsigned char CAN_received_length;
volatile unsigned char CAN_new_received_message = 0;

volatile unsigned char UART_received_data1;
volatile unsigned char UART_new_received_message1 = 0;
volatile unsigned char UART_received_data2;
volatile unsigned char UART_new_received_message2 = 0;

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
    ANSELA = 0x03;      // RA0 and RA1 remain analog for Joystick, RA2 (button) is digital
    ANSELB = 0x00;      // All Port B digital
    ANSELC = 0x00;      // All Port C digital
    ANSELD = 0x00;      // All Port D digital
    ANSELE = 0x00;      // All Port E digital
    
    TRISA0 = 1;         // RA0 defined as input for Joystick X
    TRISA1 = 1;         // RA1 defined as input for Joystick Y
    TRISA2 = 1;         // RA2 defined as input for Joystick button
    
    TRISB0 = 1;         // RB0 defined as input for Canbus interrupt
    TRISB4 = 0;         // RB4 defined as output for display data/command
    TRISB5 = 0;         // RB5 defined as output for display chip select
    TRISB6 = 0;         // RB6 defined as output for inputs chip select snapshot
    
    // SPI
    TRISD2 = 0;         // RD2 defined as output for chip selectoutput shift registers
    TRISD3 = 0;         // RD3 defined as output for chip select canbus module
    TRISD5 = 0;         // RD5 defined as output for chip  select input shift registers
    
    TRISC3 = 0;         // RC3 defined as output for SPI CH1 clock
    TRISC4 = 1;         // RC4 defined as input for SPI CH1 data in
    TRISC5 = 0;         // RC3 defined as output for SPI CH1 data out
    TRISD0 = 0;         // RD0 defined as output for SPI CH2 clock
    TRISD1 = 1;         // RD1 defined as input for SPI CH2 data in
    TRISD4 = 0;         // RD4 defined as output for SPI CH2 data out
    
    // UART
    TRISC0 = 1;         // RC0 defined as input for bluetooth state 
    TRISC1 = 1;         // RC1 defined as input for bluetooth enable
    TRISC6 = 0;         // RC6 defined as output for UART CH1 transmit
    TRISC7 = 1;         // RC7 defined as input for UART CH1 receive
    
    TRISC2 = 1;         // RC2 defined as input for DFPlayer busy
    TRISB3 = 0;         // RB3 defined as output for Speaker mute relais
    TRISD6 = 0;         // RD6 defined as output for UART CH2 transmit
    TRISD7 = 1;         // RD7 defined as input for UART CH2 receive
}

void Init_ADC(void){
    
}


void Init_Interrupts(void){
    INT0IE = 1;     // Enable external interrupt INT0
    INTEDG0 = 1;    // Set rising edge
    INT0IF = 0;     // Clear the flag
    
    // SPI interrupt initialization
    SSP1IF = 0;     // Clear SPI CH1 interrupt flag
    SSP2IF = 0;     // Clear SPI CH2 interrupt flag
    
    SSP1IE = 0;     // Disable SPI CH1 interrupt
    SSP2IE = 0;     // Disable SPI CH2 interrupt
    
    // UART enable receive interrupts
    RC1IE = 1;      // Enable UART CH1 receive interrupt
    RC2IE = 1;      // Enable UART CH2 receive interrupt
    
    // Global and Peripheral Interrupt initialization
    PEIE = 1;       // Peripheral Interrupt Enable
    GIE = 1;        // Global Interrupt Enable
}

// Interrupt serice routines

void __interrupt() Interrupts(void){    
    // Call CanBus (MCP) ISR Handler (check if CanBus message was received and handle message)
    MCP_ISR_Handler();
    
    // Call UART ISR Handler (check if message was received and handle message)
    UART_ISR_Handler();
}

void main(void) {
    // Local variable definition
    
    // Calling of Init Functions
    Init_Pins();
    Init_ADC();
    Init_Interrupts();
    Init_SPI();
    Init_UART();
    Init_Display();
    Init_MCP();
    Init_HC05();

    while(1){
        /*
        // main programm here
        switch (currentState){
        case stateInit:
            // Error handling
            
            // Initial state programm (executed once upon call)
            if(wasTransition()){
                // Initial programm here
                
                // Update lastState
                updateLastState();
            }
                    
            // State programm (cyclic)
            
            // Transititons
            
            break;
        
        case stateMenu:
            // Error handling
            
            // Initial state programm (executed once upon call)
            if(wasTransition()){
                // Initial programm here
                
                // Update lastState
                updateLastState();
            }
                    
            // State programm (cyclic)
            state_machine_menu();       // State machine of main menu, calls all further machines
            // Transititons
            
            break;
            
        case stateError:
            // Error handling
            
            // Initial state programm (executed once upon call)
            if(wasTransition()){
                // Initial programm here
                
                // Update lastState
                updateLastState();
            }
                    
            // State programm (cyclic)
            
            // Transititons
            
            break;
        
        default:
            // none
            break;
        }
        */
        
        
    }
    
    return;
}


/*  Input and Output testing block
        unsigned int read = 0;
        read = SPI_Read_Inputs();
        SPI_Write_Outputs(read);
        __delay_ms(50);
 */