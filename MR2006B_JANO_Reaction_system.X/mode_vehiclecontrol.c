/* 
 * File:                mode_vehiclecontrol.c
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    31.08.2026 16:07 - Created file
 */

// #includes
#include "globals.h"
#include "ssd1312.h"
#include "inputs.h"
#include "spi_functions.h"

// Global variable definition
typedef enum states{
    stateInit=-1,                           // -1 for startup safe
    stateGame
} state_t;                                  // Enums for state machine
static state_t currentState = stateInit;    // To save current state
static state_t lastState = stateGame;       // To save last state (edges)

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

void state_machine_vehiclecontrol(void){
    
    switch (currentState){            
        case stateInit:
            static unsigned char ready_to_start = 0;

            if(wasTransition()){
                ready_to_start = 0;
                SPI_Write_Outputs(0x0000);
                Display_Draw_Mode_Screen("Vehicle", "Start: PRESS Joystick", "Menu: HOLD Joystick");
                updateLastState();
            }

            // Wait until the menu-selection press has been released
            if(!isJoyStickPressed()){
                ready_to_start = 1;
            }

            if(ready_to_start && wasJoystickPressed()){
                transitionTo(stateGame);
            }
            break;
        
        default:
            // none
            break;
    }
    
}