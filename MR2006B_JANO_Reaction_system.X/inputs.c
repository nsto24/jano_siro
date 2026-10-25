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

// Functions

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