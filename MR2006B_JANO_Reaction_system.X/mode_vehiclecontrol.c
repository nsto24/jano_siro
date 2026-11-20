/* 
 * File:                mode_vehiclecontrol.c
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    31.08.2026 16:07 - Created file
 */

// #includes
#include "globals.h"
#include "ssd1312.h"

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
            // Error handling
            
            // Initial state programm (executed once upon call)
            if(wasTransition()){
                // Initial programm here
                Display_Draw_Mode_Screen("Vehicle", "To start the activity", "PRESS Joystick");
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
    
}