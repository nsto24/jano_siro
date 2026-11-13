/*
 * File:                input.c
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    31.08.2026, 17:59
 */

// #includes
#include "inputs.h"
#include "globals.h"
#include "mcu_config.h"
#include "adc_functions.h"
#include "spi_functions.h"

// Variable definition

// Live variables modified by the ISR
volatile unsigned int btn_state = 0;        // Saves the state of the buttons on last read
volatile unsigned int btn_pressed = 0;      // Saves which buttons were newly pressed on last read
volatile unsigned int btn_released = 0;     // Saves which buttons were released on last read
volatile unsigned char vol_live = 0;        // Saves current volume
volatile unsigned char vol_changed_live = 0;// Saves flag if volume has changed from last reading
// Snapshot variables for the main loop
static unsigned int snap_state = 0;
static unsigned int snap_pressed = 0;
static unsigned int snap_released = 0;
static unsigned char vol_snap = 0;
static unsigned char vol_changed_snap = 0;


// Functions for Joystick reading

joystick_t readJoystick(void){
    joystick_t res;
    Adc_SetChannel(ADC_CH_JOYSTICK_X);
    res.x = Adc_ReadValue();
    Adc_SetChannel(ADC_CH_JOYSTICK_Y);
    res.y = Adc_ReadValue();
    return res;
}

char isJoyStickLeft(){
    joystick_t read = readJoystick();
    return ((read.x > (1023*JOYSTICK_MID_MIN) && read.x < (1023*JOYSTICK_MID_MAX)) && (read.y < (1023*JOYSTICK_MIN)));
}

char isJoyStickRight(){
    joystick_t read = readJoystick();
    return ((read.x > (1023*JOYSTICK_MID_MIN) && read.x < (1023*JOYSTICK_MID_MAX)) && (read.y > (1023*JOYSTICK_MAX)));
}

char isJoyStickUp(){
    joystick_t read = readJoystick();
    return ((read.x > (1023*JOYSTICK_MAX)) && (read.y > (1023*JOYSTICK_MID_MIN) && read.y < (1023*JOYSTICK_MID_MAX)));
}

char isJoyStickDown(){
    joystick_t read = readJoystick();
    return ((read.x < (1023*JOYSTICK_MIN)) && (read.y > (1023*JOYSTICK_MID_MIN) && read.y < (1023*JOYSTICK_MID_MAX)));
}

char isJoyStickIdle(){
    joystick_t read = readJoystick();
    return ((read.x > (1023*JOYSTICK_MIN)) && (read.x < (1023*JOYSTICK_MAX)) && (read.y > (1023*JOYSTICK_MID_MIN) && read.y < (1023*JOYSTICK_MID_MAX)));
}

char isJoyStickPressed(){
    if(JOYSTICK_SW !=0){return 1;}
    else{return 0;}
}

float JoyStickAxisXpercent(){
    joystick_t read = readJoystick();
    return read.x/1024.0;
}

float JoyStickAxisYpercent(){
    joystick_t read = readJoystick();
    return read.y/1024.0;
}

float JoyStickPitchAxisXpercent(){
    float x = JoyStickAxisXpercent();
    return (x * 2.0) - 1.0;
}

float JoyStickPitchAxisYpercent(){
    float y = JoyStickAxisYpercent();
    return (y * 2.0) - 1.0;
}

joystick_state_t getJoystickState(){
    if(isJoyStickIdle()){return JOY_IDLE;}
    else if(isJoyStickRight()){return JOY_RIGHT;}
    else if(isJoyStickLeft()){return JOY_LEFT;}
    else if(isJoyStickUp()){return JOY_UP;}
    else if(isJoyStickDown()){return JOY_DOWN;}
    else if(isJoyStickPressed()){return JOY_PRESSED;}
}

// Functions for Volume Potentiometer reading

unsigned char getVolumeState(void) {
    return vol_snap;
}

unsigned char hasVolumeChanged(void) {
    return vol_changed_snap;
}

// Functions for Reading of buttons

void Sync_Inputs(void) {
    GIE = 0;                            // Disable interrupts for atomic transfer
    // Button snaps
    snap_state = btn_state;             // Copy absolute state
    snap_pressed = btn_pressed;         // Copy edge flags
    btn_pressed = 0;                    // Clear live edge flags safely
    snap_released = btn_released;
    btn_released = 0;
    // Volume snaps
    vol_snap = vol_live;
    vol_changed_snap = vol_changed_live;
    vol_changed_live = 0;               // Clear the live flag safely
    
    GIE = 1;                            // Re-enable interrupts
}

unsigned int getButtonState(void){
    unsigned int temp = 0;
    GIE = 0;                    // Disable global interrupts
    temp = btn_state;           
    GIE = 1;                    // Enable global interrupts
    return temp;
}

unsigned int getButtonPressed(void){
    unsigned int temp = 0;
    GIE = 0;                    // Disable global interrupts
    temp = btn_pressed;           
    GIE = 1;                    // Enable global interrupts
    return temp;
}

unsigned int getButtonReleased(void){
    unsigned int temp = 0;
    GIE = 0;                    // Disable global interrupts
    temp = btn_released;           
    GIE = 1;                    // Enable global interrupts
    return temp;
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

void Buttons_ISR_Handler(void){
    if(TMR0IE && TMR0IF){
        unsigned int btn_raw = 0;

        // Reset Timer0 registers for the next 10ms tick
        TMR0H = TMR0H_VAL;
        TMR0L = TMR0L_VAL;

        // Reading buttons from input register
        btn_raw = SPI_Read_Inputs();

        // Analysing rising edges:  Set if one bit is set among last and raw and raw is set
        btn_pressed |= (btn_state ^ btn_raw) & btn_raw;     // OR= (last XOR raw) AND raw

        // Analysing falling edges:  Set if one bit is set among last and raw and raw is not set
        btn_released |= (btn_state ^ btn_raw) & ~btn_raw;   // OR= (last XOR raw) AND NOT raw

        // Writing the read button states into the current state variable
        btn_state = btn_raw;
        
        // Volume reading
        static unsigned int last_raw_vol = 0;
        
        Adc_SetChannel(ADC_CH_VOLUME);
        unsigned int raw_vol = Adc_ReadValue();
        
        // Check if value is within certain hysteresis
        if(raw_vol > (last_raw_vol + 4) || last_raw_vol > (raw_vol + 4)){
            last_raw_vol = raw_vol;
            
            unsigned char new_vol = (unsigned char)((raw_vol * 30) / 1023);
            if (new_vol != vol_live) {
                vol_live = new_vol;
                vol_changed_live = 1; // Flag that a mathematical change occurred
            }
        }

        // Clearing interrupt flag
        TMR0IF = 0;
    }
}