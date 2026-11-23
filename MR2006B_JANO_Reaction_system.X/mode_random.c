/* 
- File:                mode_random.c
- Author:              Nicolas Stock
- Comments:            Random reaction game mode
- Revision history:    31.08.2026 16:03 - Created file
 */

// #includes
#include "globals.h"
#include "ssd1312.h"
#include "inputs.h"
#include "spi_functions.h"

#define MAX_MISSES 2

// State machine definitions
typedef enum {
    stateInit = -1,
    stateGame
} state_t;

static state_t currentState = stateInit;
static state_t lastState = stateGame;

static unsigned int success_count = 0;          // Successful button hits
static unsigned int failed_count = 0;           // Total wrong button presses
static unsigned char miss_count = 0;            // Consecutive misses on current LED
static unsigned int current_button_mask = 0;    // One-hot bitmask of active target LED

// State machine helpers
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

// Helper to reset the mode when exiting to the menu
void reset_mode_random(void){
    currentState = stateInit;
    lastState = stateGame;
    current_button_mask = 0;
    SPI_Write_Outputs(0x0000);
}

void updateModeScreen(){
    char buf[24] = {0};
    sprintf(buf, "Score:%3u Miss:%3u", success_count, failed_count);
    Display_Draw_Mode_Screen("Hold&Relea", buf, "Menu: HOLD Joystick");
}

void state_machine_random(void){
    unsigned int pressed = 0;

    switch (currentState){
        case stateInit:
            if(wasTransition()){
                SPI_Write_Outputs(0x0000);
                Display_Draw_Mode_Screen("Random", "Start: PRESS Joystick", "Menu: HOLD Joystick");
                updateLastState();
            }

            if(wasJoystickPressed()){
                transitionTo(stateGame);
            }
            break;

        case stateGame:
            if(wasTransition()){
                success_count = 0;
                failed_count = 0;
                miss_count = 0;

                current_button_mask = getNextRandomButtonMask(0);
                SPI_Write_Outputs(current_button_mask);

                updateModeScreen();
                updateLastState();
            }

            // Read single-cycle rising-edge snapshot (0 until next Timer0 ISR + Sync_Inputs)
            pressed = getButtonPressed();

            if(pressed != 0){
                if((pressed & current_button_mask) != 0){
                    // Target button pressed
                    success_count++;
                    miss_count = 0;
                    current_button_mask = getNextRandomButtonMask(current_button_mask);
                    SPI_Write_Outputs(current_button_mask);
                } else {
                    // Wrong button pressed
                    failed_count++;
                    miss_count++;
                    if(miss_count > MAX_MISSES){
                        // Missed 3rd time: reset streak and switch to a new LED
                        miss_count = 0;
                        current_button_mask = getNextRandomButtonMask(current_button_mask);
                        SPI_Write_Outputs(current_button_mask);
                    }
                }

                updateModeScreen();
            }
            break;

        default:
            break;
    }
}