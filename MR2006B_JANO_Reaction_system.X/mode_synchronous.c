/* 
- File:                mode_synchronous.c
- Author:              Nicolas Stock
- Comments:            Synchronous two-hand reaction game mode
- Revision history:    20.11.2026 09:30 - Created file
 */

// #includes
#include "globals.h"
#include "ssd1312.h"
#include "inputs.h"
#include "spi_functions.h"

#define SYNC_PAIR_COUNT     6u      // 6 corresponding button pairs across left & right 2x3 grids
#define SYNC_WINDOW_MS      250u    // Max allowed time window between left & right button presses

// Corresponding pairs: {Left 2x3 grid, Right 2x3 grid}
static const unsigned int SYNC_PAIRS[SYNC_PAIR_COUNT][2] = {
    {BUTTON_A_1, BUTTON_A_3},
    {BUTTON_A_2, BUTTON_A_4},
    {BUTTON_B_1, BUTTON_B_3},
    {BUTTON_B_2, BUTTON_B_4},
    {BUTTON_C_1, BUTTON_C_3},
    {BUTTON_C_2, BUTTON_C_4}
};

// Global variable definition
typedef enum states {
    stateInit = -1,                         // -1 for startup safe
    stateGame
} state_t;                                  // Enums for state machine

static state_t currentState = stateInit;    // To save current state
static state_t lastState = stateGame;       // To save last state (edges)

static unsigned int success_count = 0;      // Successful synchronous pair presses
static unsigned int failed_count = 0;       // Wrong presses or out-of-sync timeouts
static unsigned char current_pair_idx = 0;  // Active index into SYNC_PAIRS

static unsigned char left_hit = 0;          // Rising edge detected on active left button
static unsigned char right_hit = 0;         // Rising edge detected on active right button
static unsigned char sync_timer_active = 0; // 1 once the first button of the pair is pressed
static unsigned int sync_elapsed_ms = 0;    // Time elapsed since the first button was pressed

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

// Returns a pair index (0 .. SYNC_PAIR_COUNT-1) different from exclude_idx
static unsigned char getNextRandomPairIndex(unsigned char exclude_idx){
    unsigned char next_idx;
    do {
        next_idx = (unsigned char)(rand() % SYNC_PAIR_COUNT);
    } while (next_idx == exclude_idx && SYNC_PAIR_COUNT > 1);
    return next_idx;
}

// Reset helper to call when exiting back to the main menu
void reset_mode_synchronous(void){
    currentState = stateInit;
    lastState = stateGame;
    left_hit = 0;
    right_hit = 0;
    sync_timer_active = 0;
    sync_elapsed_ms = 0;
    SPI_Write_Outputs(0x0000);
}

static void updateModeScreen(){
    char buf[24] = {0};
    sprintf(buf, "Score:%3u Miss:%3u", success_count, failed_count);
    Display_Draw_Mode_Screen("Hold&Relea", buf, "Menu: HOLD Joystick");
}

void state_machine_synchronous(void){
    char buf[24] = {0};
    unsigned int pressed = 0;

    switch (currentState){            
        case stateInit:
            static unsigned char ready_to_start = 0;

            if(wasTransition()){
                ready_to_start = 0;
                SPI_Write_Outputs(0x0000);
                Display_Draw_Mode_Screen("Synchron", "Start: PRESS Joystick", "Menu: HOLD Joystick");
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
                left_hit = 0;
                right_hit = 0;
                sync_timer_active = 0;
                sync_elapsed_ms = 0;

                current_pair_idx = getNextRandomPairIndex(0xFF);
                SPI_Write_Outputs(SYNC_PAIRS[current_pair_idx][0] | SYNC_PAIRS[current_pair_idx][1]);

                updateModeScreen();
                updateLastState();
            }

            unsigned int left_mask = SYNC_PAIRS[current_pair_idx][0];
            unsigned int right_mask = SYNC_PAIRS[current_pair_idx][1];
            unsigned int pair_mask = left_mask | right_mask;

            // Read single-cycle rising-edge snapshot
            pressed = getButtonPressed();

            if((pressed & ~pair_mask) != 0){
                // Any non-target button pressed -> penalize and reset current sync attempt
                failed_count++;
                left_hit = 0;
                right_hit = 0;
                sync_timer_active = 0;
                sync_elapsed_ms = 0;

                updateModeScreen();
            }
            else {
                // Latch rising edges on the left and right target buttons
                if((pressed & left_mask) != 0){
                    left_hit = 1;
                    if(!sync_timer_active){
                        sync_timer_active = 1;
                        sync_elapsed_ms = 0;
                    }
                }
                if((pressed & right_mask) != 0){
                    right_hit = 1;
                    if(!sync_timer_active){
                        sync_timer_active = 1;
                        sync_elapsed_ms = 0;
                    }
                }

                if(sync_timer_active){
                    if(left_hit && right_hit && isButtonPressed(left_mask) && isButtonPressed(right_mask)){
                        // Both buttons pressed and held within SYNC_WINDOW_MS
                        success_count++;
                        left_hit = 0;
                        right_hit = 0;
                        sync_timer_active = 0;
                        sync_elapsed_ms = 0;

                        current_pair_idx = getNextRandomPairIndex(current_pair_idx);
                        SPI_Write_Outputs(SYNC_PAIRS[current_pair_idx][0] | SYNC_PAIRS[current_pair_idx][1]);

                        updateModeScreen();
                    }
                    else if((sync_elapsed_ms >= SYNC_WINDOW_MS) ||
                            (left_hit && !isButtonPressed(left_mask)) ||
                            (right_hit && !isButtonPressed(right_mask))){
                        // Timed out or released the first button before pressing the second
                        failed_count++;
                        left_hit = 0;
                        right_hit = 0;
                        sync_timer_active = 0;
                        sync_elapsed_ms = 0;

                        current_pair_idx = getNextRandomPairIndex(current_pair_idx);
                        SPI_Write_Outputs(SYNC_PAIRS[current_pair_idx][0] | SYNC_PAIRS[current_pair_idx][1]);

                        updateModeScreen();
                    }
                    else {
                        sync_elapsed_ms += TARGET_MS;
                    }
                }
            }
            break;

        default:
            break;
    }
}