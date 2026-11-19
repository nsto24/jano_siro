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

// Local variables for menu state

// Global variable definition
typedef enum states{
    stateInit=-1,                   // -1 for startup safe
    stateMenuPage1,
    stateMenuPage2,
    stateModeFollower,
    stateModeRandom,
    stateModeSynchronous,
    stateModeVehicleControl
} state_t;                                  // Enums for state machine
static state_t currentState = stateInit;    // To save current state
static state_t lastState = stateInit;       // To save last state (edges)

// Definition of Datatypes for Menu
typedef struct{
    const char *label;
}MenuItem_t;

typedef struct{
    const char *title;
    unsigned char itemCount;
    MenuItem_t items[MAX_ITEMS_PER_PAGE];
}MenuPage_t;

typedef struct{
    const MenuPage_t *pages;
    unsigned char pageCount;
    unsigned char currentPage;
    unsigned char selectedItem;
}MenuSystem_t;

// Declaration of menu structure
const MenuPage_t MenuPages[] = {
    {
        .title = "GAME MODES",
        .itemCount = 4,
        .items = {
            { "Random Mode"},
            { "Hold & Release"},
            { "Synchronous Mode"},
            { "Vehicle Control"}
        }
    },
    {
        .title = "SETTINGS",
        .itemCount = 1,
        .items = {
            { "Speaker Mute"}
        }
    }
};

MenuSystem_t menu = {
    .pages = MenuPages,
    .pageCount = sizeof(MenuPages)/sizeof(MenuPage_t),
    .currentPage = 0,
    .selectedItem = 0
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

// Redraw function of menu

void redraw_menu_page(const MenuSystem_t *menu){
    const MenuPage_t *activePage = &menu->pages[menu->currentPage];
    
    Display_Clear();
    Display_Draw_Headbar();
    Display_Draw_String(20, DISPLAY_HEADBAR_WIDTH + 2, (char *)activePage->title, &Font_5x7, WHITE);
    
    for(unsigned char i = 0; i < activePage->itemCount; i++){
        unsigned char yPos = DISPLAY_HEADBAR_WIDTH + 10 + (i * 10);
        if(menu->selectedItem == i){
            Display_Draw_Icon(5, yPos, 8, 8, icon_arrow, WHITE);
        }
        Display_Draw_String(15, yPos, (char *)activePage->items[i].label, &Font_5x7, WHITE);
    }
    
    // Set redraw flag to update OLED in main loop
    FLAG_redraw_display = 1;
}

// Handles vertical cursor movement only when inside a menu page
static void handle_vertical_navigation(void){
    unsigned char count = menu.pages[menu.currentPage].itemCount;
    if(count == 0) return;

    if(wasJoystickDown()){
        if((menu.selectedItem + 1) < count){
            menu.selectedItem++;
        } else {
            menu.selectedItem = 0;
        }
        redraw_menu_page(&menu);
    }
    else if(wasJoystickUp()){
        if(menu.selectedItem > 0){
            menu.selectedItem--;
        } else {
            menu.selectedItem = count - 1;
        }
        redraw_menu_page(&menu);
    }
}

void state_machine_menu(void){
    switch (currentState){
        case stateInit:
            if(wasTransition()){
                updateLastState();
            }
            transitionTo(stateMenuPage1);
            break;
        
        case stateMenuPage1:
            if(wasTransition()){
                menu.currentPage = 0;
                menu.selectedItem = 0;
                redraw_menu_page(&menu);
                updateLastState();
            }
            
            handle_vertical_navigation();
            
            // Transitions
            if(wasJoystickRight()){
                transitionTo(stateMenuPage2);
            }
            else if(wasJoystickPressed()){
                switch(menu.selectedItem){
                    case 0: transitionTo(stateModeRandom);         break;
                    case 1: transitionTo(stateModeFollower);       break;
                    case 2: transitionTo(stateModeSynchronous);    break;
                    case 3: transitionTo(stateModeVehicleControl); break;
                    default: break;
                }
            }
            break;
            
        case stateMenuPage2:
            if(wasTransition()){
                menu.currentPage = 1;
                menu.selectedItem = 0;
                redraw_menu_page(&menu);
                updateLastState();
            }
            
            handle_vertical_navigation();
            
            // Transitions & Actions
            if(wasJoystickLeft()){
                transitionTo(stateMenuPage1);
            }
            else if(wasJoystickPressed() && menu.selectedItem == 0){
                SPEAKER_MUTE = !SPEAKER_MUTE;
                redraw_menu_page(&menu);
            }
            break;
        
        default:
            // Active mode states (add exit condition back to stateMenuPage1 here later)
            break;
    }    
}
