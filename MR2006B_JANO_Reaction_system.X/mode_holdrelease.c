/* 
- File:                mode_holdrelease.c
- Author:              Nicolas Stock
- Comments:            Hold & Release chained reaction game mode
- Revision history:    31.08.2026 16:06 - Created file
 */

// #includes
#include "globals.h"
#include "ssd1312.h"
#include "inputs.h"
#include "spi_functions.h"

// State machine definitions
typedef enum {
    stateInit = -1,                         // -1 for startup safe
    stateGame
} state_t;

typedef enum {
    PHASE_WAIT_ANCHOR = 0,                  // 1 LED lit: waiting for player to press & hold anchor
    PHASE_WAIT_TARGET                       // 2 LEDs lit: holding anchor, waiting for target press
} hold_phase_t;

static state_t currentState = stateInit;
static state_t lastState = stateGame;
static hold_phase_t currentPhase = PHASE_WAIT_ANCHOR;

static unsigned int success_count = 0;      // Successful chained presses
static unsigned int failed_count = 0;       // Wrong presses or premature anchor releases
static unsigned int anchor_mask = 0;        // Currently held button bitmask
static unsigned int target_mask = 0;        // Next button bitmask to press

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

// Returns a one-hot bitmask (0x0001 .. 1<<(BUTTON_COUNT-1)) different from exclude_mask
static unsigned int getNextRandomButtonMask(unsigned int exclude_mask){
    unsigned int next_mask;
    do {
        next_mask = (unsigned int)(1u << (rand() % BUTTON_COUNT));
    } while (next_mask == exclude_mask && BUTTON_COUNT > 1);
    return next_mask;
}

// Reset helper to call when exiting back to the main menu
void reset_mode_holdrelease(void){
    currentState = stateInit;
    lastState = stateGame;
    currentPhase = PHASE_WAIT_ANCHOR;
    anchor_mask = 0;
    target_mask = 0;
    SPI_Write_Outputs(0x0000);
}

static void updateModeScreen(){
    char buf[24] = {0};
    sprintf(buf, "Score:%3u Miss:%3u", success_count, failed_count);
    Display_Draw_Mode_Screen("Hold&Relea", buf, "Menu: HOLD Joystick");
}

void state_machine_holdrelease(void){
    unsigned int pressed = 0;

    switch (currentState){
        case stateInit:
            static unsigned char ready_to_start = 0;

            if(wasTransition()){
                ready_to_start = 0;
                SPI_Write_Outputs(0x0000);
                Display_Draw_Mode_Screen("Hold&Relea", "Start: PRESS Joystick", "Menu: HOLD Joystick");
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

        case stateGame:
            if(wasTransition()){
                success_count = 0;
                failed_count = 0;
                currentPhase = PHASE_WAIT_ANCHOR;

                anchor_mask = getNextRandomButtonMask(0);
                target_mask = 0;
                SPI_Write_Outputs(anchor_mask);

                updateModeScreen();
                updateLastState();
            }

            // Read single-cycle rising-edge snapshot
            pressed = getButtonPressed();

            if(currentPhase == PHASE_WAIT_ANCHOR){
                if((pressed & anchor_mask) != 0){
                    // Anchor button pressed: light up second button while anchor is held
                    target_mask = getNextRandomButtonMask(anchor_mask);
                    SPI_Write_Outputs(anchor_mask | target_mask);
                    currentPhase = PHASE_WAIT_TARGET;
                }
                else if(pressed != 0){
                    // Wrong button pressed before grabbing anchor
                    failed_count++;
                    updateModeScreen();
                }
            }
            else { // PHASE_WAIT_TARGET
                if(!isButtonPressed(anchor_mask)){
                    // Anchor button was released before pressing target: break chain & penalize
                    failed_count++;
                    target_mask = 0;
                    SPI_Write_Outputs(anchor_mask);
                    currentPhase = PHASE_WAIT_ANCHOR;

                    updateModeScreen();
                }
                else if((pressed & target_mask) != 0){
                    // Both anchor and target are pressed: shift target -> new anchor, light new target
                    success_count++;
                    anchor_mask = target_mask;
                    target_mask = getNextRandomButtonMask(anchor_mask);
                    SPI_Write_Outputs(anchor_mask | target_mask);

                    updateModeScreen();
                }
                else if((pressed & ~anchor_mask) != 0){
                    // Wrong third button pressed while holding anchor
                    failed_count++;
                    updateModeScreen();
                }
            }
            break;

        default:
            break;
    }
}