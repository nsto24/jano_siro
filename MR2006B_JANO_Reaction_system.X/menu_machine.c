/* 
 * File:                menu_machine.c
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    31.08.2026 17:03 - Created file
 */

// #includes
#include "globals.h"

// Global variable definition
typedef enum states{
    stateInit=-1                    // -1 for startup safe
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

void state_machine_menu(void){
    
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
        
        default:
            // none
            break;
    }
    
}
