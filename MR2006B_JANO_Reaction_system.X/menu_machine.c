/* 
 * File:                menu_machine.c
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    31.08.2026 17:03 - Created file
 */

// #includes
#include "globals.h"
#include "inputs.h"
#include "ssd1312.h"

// Global variable definition
typedef enum states{
    stateInit=-1,                   // -1 for startup safe
    stateMainMenu,
    stateChainModeSelected,
    stateHoldReleaseModeSelected,
    stateRandomModeSelected,
    stateVehicleControlSelected,
    stateSettingsSelected
} state_t;                                  // Enums for state machine
static state_t currentState = stateInit;    // To save current state
static state_t lastState = stateInit;       // To save last state (edges)

// Local variables for menu state
static const unsigned char max_items = 5;
static unsigned char selected_item = 0;
static unsigned char display_redraw = 0; 
static joystick_state_t current_joystick = JOY_IDLE;
static joystick_state_t last_joystick = JOY_IDLE;

// Main Menu Strings Definition
const char* main_menu_strings[] = {
    "Chain Mode",
    "Hold & Release Mode",
    "Random Mode",
    "Vehicle Control",
    "Settings"
};

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
            transitionTo(stateMainMenu);
            // Transititons
            
            break;
        
        case stateMainMenu:
            // Error handling
            
            // Initial state programm (executed once upon call)
            if(wasTransition()){
                // Initial programm here
                display_redraw = 1;
                // Update lastState
                updateLastState();
            }
                    
            // State programm (cyclic)
            current_joystick = getJoystickState();            
            
            if(current_joystick == JOY_DOWN && last_joystick != JOY_DOWN){
                if (selected_item < max_items - 1) {
                    selected_item++;
                } else {
                    selected_item = 0; // Wrap to top
                }
                display_redraw = 1;
            }
            if(current_joystick == JOY_UP && last_joystick != JOY_UP){
                if (selected_item > 0) {
                    selected_item--;
                } else {
                    selected_item = max_items - 1; // Wrap to bottom
                }
                display_redraw = 1;
            }
            
            if (display_redraw) {
                Display_Clear(); // Wipes local buffer

                // Draw the static UI headbar at the top
                Display_Draw_Headbar(1); 

                // Render menu items dynamically
                for (unsigned char i = 0; i < max_items; i++) {
                    // Calculate Y position: Starts at pixel 15, spaced by 10 pixels
                    unsigned int y_pos = 15 + (i * 10); 

                    // Draw item text offset slightly to the right to leave room for the arrow
                    Display_Draw_String(12, y_pos, (char*)main_menu_strings[i], &Font_5x7, WHITE);

                    // Draw Cursor Arrow next to the currently selected item
                    if (i == selected_item) {
                        Display_Draw_Icon(2, y_pos, 8, 8, icon_arrow, WHITE);
                    }
                }

                Display_Update(); // Push buffer to OLED
                display_redraw = 0; // Clear flag
            }
            
            // Transititons
            if(current_joystick == JOY_PRESSED && last_joystick != JOY_PRESSED){
                if(selected_item == 0){transitionTo(stateChainModeSelected);}
                if(selected_item == 1){transitionTo(stateHoldReleaseModeSelected);}
                if(selected_item == 2){transitionTo(stateRandomModeSelected);}
                if(selected_item == 3){transitionTo(stateVehicleControlSelected);}
                if(selected_item == 4){transitionTo(stateSettingsSelected);}
            }
            
            
            break;
        
        default:
            // none
            break;
    }
    
    last_joystick = current_joystick;
    
}
