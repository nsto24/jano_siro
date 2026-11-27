/*
- File:                inputs.c
- Author:              Nicolas Stock
- Comments:            Integrated Joystick into ISR and Snapshot architecture (with 50ms hold buffer)
- Revision history:    31.08.2026, 17:59
 */

// #includes
#include "inputs.h"
#include "globals.h"
#include "mcu_config.h"
#include "adc_functions.h"
#include "spi_functions.h"

// Compile-time integer ADC thresholds for Joystick evaluation
#define JOY_TH_MIN              ((unsigned int)(1023.0 * JOYSTICK_MIN))
#define JOY_TH_MAX              ((unsigned int)(1023.0 * JOYSTICK_MAX))
#define JOY_TH_MID_MIN          ((unsigned int)(1023.0 * JOYSTICK_MID_MIN))
#define JOY_TH_MID_MAX          ((unsigned int)(1023.0 * JOYSTICK_MID_MAX))
#define JOY_HYSTERESIS          15u

// Minimum hold duration: 5 ticks * 10ms Timer0 ISR = 50ms
#define JOY_HOLD_FILTER_TICKS       5u
#define JOY_RELEASE_FILTER_TICKS    2u


// Variable definition

// Live variables modified by the ISR
volatile unsigned int btn_state = 0;                // Saves the state of the buttons on last read
volatile unsigned int btn_pressed = 0;              // Saves which buttons were newly pressed on last read
volatile unsigned int btn_released = 0;             // Saves which buttons were released on last read
volatile unsigned char vol_live = 0;                // Saves current volume
volatile unsigned char vol_changed_live = 0;        // Saves flag if volume has changed from last reading
volatile joystick_t joy_live = {0, 0, 0};           // Saves buffered state of Joystick 
volatile unsigned char joy_sw_pressed_live = 0;  
volatile unsigned char joy_sw_released_live = 0; 
volatile joystick_state_t joy_dir_live = JOY_IDLE;
volatile joystick_state_t joy_dir_edge_live = JOY_IDLE;
volatile unsigned int joy_sw_hold_ticks_live = 0;   // Counts 10ms ticks while joystick SW is held

// Snapshot variables for the main loop
static unsigned int snap_state = 0;
static unsigned int snap_pressed = 0;
static unsigned int snap_released = 0;
static unsigned char snap_vol = 0;
static unsigned char snap_vol_changed = 0;
static joystick_t snap_joy = {0, 0, 0};
static unsigned char snap_joy_sw_pressed = 0;    
static unsigned char snap_joy_sw_released = 0;   
static joystick_state_t snap_joy_dir = JOY_IDLE;
static joystick_state_t snap_joy_dir_edge = JOY_IDLE;
static unsigned int snap_joy_sw_hold_ticks = 0;
static unsigned int snap_joy_sw_prev_hold_ticks = 0;


// ==========================================================================
// Functions for Joystick evaluation (Using Snapshot Data)
// ==========================================================================

static joystick_state_t evaluateJoystickDirection(unsigned int x, unsigned int y, joystick_state_t currentDir){
    // Apply hysteresis release thresholds if already active in a cardinal direction
    switch(currentDir){
        case JOY_LEFT:
            if(y < (JOY_TH_MIN + JOY_HYSTERESIS)){ return JOY_LEFT; }
            break;
        case JOY_RIGHT:
            if(y > (JOY_TH_MAX - JOY_HYSTERESIS)){ return JOY_RIGHT; }
            break;
        case JOY_UP:
            if(x > (JOY_TH_MAX - JOY_HYSTERESIS)){ return JOY_UP; }
            break;
        case JOY_DOWN:
            if(x < (JOY_TH_MIN + JOY_HYSTERESIS)){ return JOY_DOWN; }
            break;
        default:
            break;
    }

    // Compare deflection from center (512) so slight off-center drift on the other axis doesn't block transitions
    unsigned int dx = (x >= 512u) ? (x - 512u) : (512u - x);
    unsigned int dy = (y >= 512u) ? (y - 512u) : (512u - y);

    if(dy > dx){
        if(y < JOY_TH_MIN){ return JOY_LEFT; }
        if(y > JOY_TH_MAX){ return JOY_RIGHT; }
    } else {
        if(x > JOY_TH_MAX){ return JOY_UP; }
        if(x < JOY_TH_MIN){ return JOY_DOWN; }
    }

    return JOY_IDLE;
}

unsigned char isJoyStickLeft(void){
    return (snap_joy_dir == JOY_LEFT);
}

unsigned char isJoyStickRight(void){
    return (snap_joy_dir == JOY_RIGHT);
}

unsigned char isJoyStickUp(void){
    return (snap_joy_dir == JOY_UP);
}

unsigned char isJoyStickDown(void){
    return (snap_joy_dir == JOY_DOWN);
}

unsigned char isJoyStickIdle(void){
    return (snap_joy_dir == JOY_IDLE);
}

unsigned char isJoyStickPressed(void){
    // Read from the synchronized snapshot, not the live pin
    if(snap_joy.sw != 0){ return 1; }
    else{ return 0; }
}

float getJoyStickAxisXpercent(void){
    return snap_joy.x / 1024.0;
}

float getJoyStickAxisYpercent(void){
    return snap_joy.y / 1024.0;
}

float getJoyStickPitchAxisXpercent(void){
    float x = getJoyStickAxisXpercent();
    return (x * 2.0) - 1.0;
}

float getJoyStickPitchAxisYpercent(void){
    float y = getJoyStickAxisYpercent();
    return (y * 2.0) - 1.0;
}

joystick_state_t getJoystickState(void){
    if(isJoyStickPressed()){return JOY_PRESSED;}
    else if(isJoyStickIdle()){return JOY_IDLE;}
    else if(isJoyStickRight()){return JOY_RIGHT;}
    else if(isJoyStickLeft()){return JOY_LEFT;}
    else if(isJoyStickUp()){return JOY_UP;}
    else if(isJoyStickDown()){return JOY_DOWN;}
    return JOY_IDLE; // Fallback
}

joystick_state_t getJoystickDirection(void){
    return snap_joy_dir;
}

joystick_state_t getJoystickDirectionEdge(void){
    return snap_joy_dir_edge;
}

unsigned char wasJoystickLeft(void){
    return (snap_joy_dir_edge == JOY_LEFT);
}

unsigned char wasJoystickRight(void){
    return (snap_joy_dir_edge == JOY_RIGHT);
}

unsigned char wasJoystickUp(void){
    return (snap_joy_dir_edge == JOY_UP);
}

unsigned char wasJoystickDown(void){
    return (snap_joy_dir_edge == JOY_DOWN);
}

unsigned char wasJoystickPressed(void){
    return snap_joy_sw_pressed;
}

unsigned char wasJoystickReleased(void){
    return snap_joy_sw_released;
}

unsigned int getJoystickHoldTimeMs(void){
    return snap_joy_sw_hold_ticks * INPUT_TICK_MS;
}

unsigned char isJoystickPressedFor(unsigned int duration_ms){
    unsigned int required_ticks = duration_ms / INPUT_TICK_MS;
    if(required_ticks == 0u){ return isJoyStickPressed(); }
    return (snap_joy_sw_hold_ticks >= required_ticks) ? 1 : 0;
}

unsigned char wasJoystickPressedFor(unsigned int duration_ms){
    unsigned int required_ticks = duration_ms / INPUT_TICK_MS;
    if(required_ticks == 0u){ return wasJoystickPressed(); }
    return (snap_joy_sw_hold_ticks >= required_ticks && snap_joy_sw_prev_hold_ticks < required_ticks) ? 1 : 0;
}


// ==========================================================================
// Functions for Volume Potentiometer reading
// ==========================================================================

unsigned char getVolumeState(void) {
    return snap_vol;
}

unsigned char hasVolumeChanged(void) {
    return snap_vol_changed;
}

unsigned char isVolumeZero(void){
    return snap_vol <= 0;
}

unsigned char isVolumeLow(void){
    return snap_vol > 0 && snap_vol <= 10;
}

unsigned char isVolumeMid(void){
    return snap_vol > 10  && snap_vol <= 20;
}

unsigned char isVolumeHigh(void){
    return snap_vol > 20  && snap_vol <= 30;
}

// ==========================================================================
// Functions for Reading of buttons and Syncing
// ==========================================================================

void Sync_Inputs(void) {
    GIE = 0;                            // Disable interrupts for atomic transfer
    
    // Button snaps
    snap_state = btn_state;             
    snap_pressed = btn_pressed;         
    btn_pressed = 0;                    
    snap_released = btn_released;
    btn_released = 0;
    
    // Volume snaps
    snap_vol = vol_live;
    snap_vol_changed = vol_changed_live;
    vol_changed_live = 0;               
    
    // Joystick snaps
    snap_joy = joy_live;                
    snap_joy_sw_pressed = joy_sw_pressed_live;
    joy_sw_pressed_live = 0;            // Clear live flag safely
    snap_joy_sw_released = joy_sw_released_live;
    joy_sw_released_live = 0;           // Clear live flag safely
    snap_joy_dir = joy_dir_live;
    snap_joy_dir_edge = joy_dir_edge_live;
    joy_dir_edge_live = JOY_IDLE;       // Clear live direction edge safely
    snap_joy_sw_prev_hold_ticks = snap_joy_sw_hold_ticks;
    snap_joy_sw_hold_ticks = joy_sw_hold_ticks_live;
    
    GIE = 1;                            // Re-enable interrupts
}

unsigned int getButtonState(void){
    // Reading from snap_state does not require GIE disabling since Sync_Inputs handles it
    return snap_state;
}

unsigned int getButtonPressed(void){
    return snap_pressed;
}

unsigned int getButtonReleased(void){
    return snap_released;
}

unsigned char isButtonPressed(unsigned int mask){
    return (snap_state & mask) != 0;
}

unsigned char wasButtonPressed(unsigned int mask){
    return (snap_pressed & mask) != 0;
}

unsigned char wasButtonReleased(unsigned int mask){
    return (snap_released & mask) != 0;
}

unsigned char wasAnyButtonPressed(){
    return snap_pressed != 0;
}


// ==========================================================================
// Timer0 Interrupt Service Routine (10ms Tick)
// ==========================================================================

void Buttons_ISR_Handler(void){
    if(TMR0IE && TMR0IF){
        unsigned int btn_raw = 0;
        joystick_t joy_raw = {0, 0, 0};

        // Reset Timer0 registers for the next 10ms tick
        TMR0H = TMR0H_VAL;
        TMR0L = TMR0L_VAL;

        // 1. Reading buttons from input register (SPI)
        btn_raw = SPI_Read_Inputs();

        // Analysing rising edges: Set if one bit is set among last and raw and raw is set
        btn_pressed |= (btn_state ^ btn_raw) & btn_raw;     // OR= (last XOR raw) AND raw

        // Analysing falling edges: Set if one bit is set among last and raw and raw is not set
        btn_released |= (btn_state ^ btn_raw) & ~btn_raw;   // OR= (last XOR raw) AND NOT raw

        // Writing the read button states into the current state variable
        btn_state = btn_raw;
        
        // 2. Volume reading (ADC)
        static unsigned int last_raw_vol = 0;
        
        Adc_SetChannel(ADC_CH_VOLUME);
        unsigned int raw_vol = (unsigned int)Adc_ReadValue();
        
        // Check if value is within certain hysteresis
        if(raw_vol > (last_raw_vol + 4) || last_raw_vol > (raw_vol + 4)){
            last_raw_vol = raw_vol;
            
            unsigned char new_vol = (unsigned char)((raw_vol * 30) / 1023);
            if (new_vol != vol_live) {
                vol_live = new_vol;
                vol_changed_live = 1; // Flag that a mathematical change occurred
            }
        }
        
        // 3. Read Joystick (ADC + Digital) with 50ms hold buffer
        static unsigned char joy_sw_debounced = 0u;
        static unsigned char joy_sw_integrator = 0u;
        static joystick_state_t joy_dir_candidate = JOY_IDLE;
        static unsigned char joy_dir_filter_cnt = 0u;

        Adc_SetChannel(ADC_CH_JOYSTICK_X);
        joy_raw.x = Adc_ReadValue();
        
        Adc_SetChannel(ADC_CH_JOYSTICK_Y);
        joy_raw.y = Adc_ReadValue();
        
        // Joystick switch is active-low against ground
        unsigned char raw_sw = (JOYSTICK_SW == 0) ? 1u : 0u;
        
        // Integrating 50ms press buffer (tolerates single-tick bounce without resetting)
        if(raw_sw != 0u){
            if(joy_sw_integrator < JOY_HOLD_FILTER_TICKS){
                joy_sw_integrator++;
            }
            if((joy_sw_integrator >= JOY_HOLD_FILTER_TICKS) && (joy_sw_debounced == 0u)){
                joy_sw_debounced = 1u;
                joy_sw_pressed_live = 1u;
                joy_sw_hold_ticks_live = JOY_HOLD_FILTER_TICKS; // Already held for 50ms
            } else if(joy_sw_debounced != 0u){
                if(joy_sw_hold_ticks_live < 6553u){
                    joy_sw_hold_ticks_live++;
                }
            }
        } else {
            if(joy_sw_integrator > 0u){
                joy_sw_integrator--;
            }
            if((joy_sw_integrator <= (JOY_HOLD_FILTER_TICKS - JOY_RELEASE_FILTER_TICKS)) && (joy_sw_debounced != 0u)){
                joy_sw_debounced = 0u;
                joy_sw_integrator = 0u;
                joy_sw_released_live = 1u;
                joy_sw_hold_ticks_live = 0u;
            }
        }
        joy_raw.sw = joy_sw_debounced;
        
        // Buffer cardinal direction changes: require 5 ticks (50ms) in the new direction
        joystick_state_t raw_joy_dir = evaluateJoystickDirection((unsigned int)joy_raw.x, (unsigned int)joy_raw.y, joy_dir_live);
        if(raw_joy_dir != joy_dir_live){
            if(raw_joy_dir == joy_dir_candidate){
                joy_dir_filter_cnt++;
            } else {
                joy_dir_candidate = raw_joy_dir;
                joy_dir_filter_cnt = 1u;
            }

            if(joy_dir_filter_cnt >= JOY_HOLD_FILTER_TICKS){
                joy_dir_live = joy_dir_candidate;
                joy_dir_filter_cnt = 0u;
                if(joy_dir_live != JOY_IDLE){
                    joy_dir_edge_live = joy_dir_live;
                }
            }
        } else {
            joy_dir_candidate = joy_dir_live;
            joy_dir_filter_cnt = 0u;
        }
        
        // Write the local struct to the live global struct
        joy_live = joy_raw;

        // Clearing interrupt flag
        TMR0IF = 0;
    }
}