/* 
- File:                inputs.h
- Author:              Nicolas Stock
- Comments:
- Revision history:    31.08.2026 17:09 - Created file
 */


#ifndef INPUTS_H
#define	INPUTS_H

// #includes
#include "globals.h"

// #defines
#define JOYSTICK_MIN        0.1     /* Threshold under which minimal Joystick position is detected */
#define JOYSTICK_MAX        0.9     /* Threshold above which maximal Joystick position is detected */
#define JOYSTICK_MID_MIN    0.4     /* Threshold above which middle Joystick position is detected */
#define JOYSTICK_MID_MAX    0.6     /* Threshold under which middle Joystick position is detected */

#define BUTTON_A_1          0x0001
#define BUTTON_A_2          0x0002
#define BUTTON_A_3          0x0004
#define BUTTON_A_4          0x0008
#define BUTTON_B_1          0x0010
#define BUTTON_B_2          0x0020
#define BUTTON_B_3          0x0040
#define BUTTON_B_4          0x0080
#define BUTTON_C_1          0x0100
#define BUTTON_C_2          0x0200
#define BUTTON_C_3          0x0400
#define BUTTON_C_4          0x0800

// Global variable definition
typedef struct{
    int x;
    int y;
    char sw;
} joystick_t;

typedef enum{
    JOY_IDLE = 0,
    JOY_LEFT,
    JOY_RIGHT,
    JOY_UP,
    JOY_DOWN,
    JOY_PRESSED
} joystick_state_t;

// Joystick Functions
/*
- The Joystick's axis are placed as:
- X: vertical axis, referred to in its extremes as UP and DOWN
- Y: horizontal axis, referred to in its extremes as LEFT and RIGHT
 */

/**
    <p><b>Function prototype: joystick_t readJoystick(void){}</b></p>
    <p><b>Summary: Reads values of Joystick X and Y axis</b></p>
    <p><b>Description: Selects channels, lets ADC read, returns values</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: joystick_t</b></p>
 */
joystick_t readJoystick(void);

/**
    <p><b>Function prototype: char isJoyStickLeft(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "LEFT" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
unsigned char isJoyStickLeft(void);

/**
    <p><b>Function prototype: char isJoyStickRight(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "RIGHT" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
unsigned char isJoyStickRight(void);

/**
    <p><b>Function prototype: char isJoyStickUp(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "UP" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
unsigned char isJoyStickUp(void);

/**
    <p><b>Function prototype: char isJoyStickDown(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "DOWN" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
unsigned char isJoyStickDown(void);

/**
    <p><b>Function prototype: char isJoyStickIdle(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "IDLE" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
unsigned char isJoyStickIdle(void);

/**
    <p><b>Function prototype: char isJoyStickPressed(void){}</b></p>
    <p><b>Summary: Checks if Joystick is pressed down center</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
unsigned char isJoyStickPressed(void);

/**
    <p><b>Function prototype: float JoyStickAxisXpercent(void){}</b></p>
    <p><b>Summary: Asses X axis value of Joystick in percent</b></p>
    <p><b>Description: From left=0.0 to right=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float getJoyStickAxisXpercent(void);

/**
    <p><b>Function prototype: float JoyStickAxisYpercent(void){}</b></p>
    <p><b>Summary: Asses Y axis value of Joystick in percent</b></p>
    <p><b>Description: From down=0.0 to up=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float getJoyStickAxisYpercent(void);

/**
    <p><b>Function prototype: float JoyStickPitchAxisXpercent(void){}</b></p>
    <p><b>Summary: Assesses X axis value of Joystick's pitch in percent</b></p>
    <p><b>Description: From middle=0.0 to down=-1.0 and up=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float getJoyStickPitchAxisXpercent(void);

/**
    <p><b>Function prototype: float JoyStickPitchAxisYpercent(void){}</b></p>
    <p><b>Summary: Assesses Y axis value of Joystick's pitch in percent</b></p>
    <p><b>Description: From middle=0.0 to left=-1.0 and right=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float getJoyStickPitchAxisYpercent(void);

joystick_state_t getJoystickState(void);

joystick_state_t getJoystickDirection(void);

joystick_state_t getJoystickDirectionEdge(void);

unsigned char wasJoystickLeft(void);

unsigned char wasJoystickRight(void);

unsigned char wasJoystickUp(void);

unsigned char wasJoystickDown(void);

unsigned char wasJoystickPressed(void);

unsigned char wasJoystickReleased(void);

float JoyStickPitchAxisYpercent(void);

/**
    <p><b>Function prototype: unsigned int getJoystickHoldTimeMs(void)</b></p>
    <p><b>Summary: Returns how long the joystick button has been held down in ms</b></p>
    <p><b>Returns: unsigned int (0 if not pressed, capped at 65530 ms)</b></p>
 */
unsigned int getJoystickHoldTimeMs(void);

/**
    <p><b>Function prototype: unsigned char isJoystickPressedFor(unsigned int duration_ms)</b></p>
    <p><b>Summary: Checks if the joystick button is currently held for at least duration_ms</b></p>
    <p><b>Parameters: duration_ms - Minimum hold time in milliseconds</b></p>
    <p><b>Returns: unsigned char (1 while held >= duration_ms, else 0)</b></p>
 */
unsigned char isJoystickPressedFor(unsigned int duration_ms);

/**
    <p><b>Function prototype: unsigned char wasJoystickPressedFor(unsigned int duration_ms)</b></p>
    <p><b>Summary: Single-shot trigger that fires once when hold time reaches duration_ms</b></p>
    <p><b>Parameters: duration_ms - Target hold time in milliseconds</b></p>
    <p><b>Returns: unsigned char (1 on the cycle duration_ms is reached, else 0)</b></p>
 */
unsigned char wasJoystickPressedFor(unsigned int duration_ms);

unsigned char getVolumeState(void);

unsigned char hasVolumeChanged(void);

unsigned char isVolumeZero(void);

unsigned char isVolumeLow(void);

unsigned char isVolumeMid(void);

unsigned char isVolumeHigh(void);

void Sync_Inputs(void);

unsigned int getButtonState(void);

unsigned int getButtonPressed(void);

unsigned int getButtonReleased(void);

unsigned char isButtonPressed(unsigned int mask);

unsigned char wasButtonPressed(unsigned int mask);

unsigned char wasButtonReleased(unsigned int mask);

void Buttons_ISR_Handler(void);

#endif	/* INPUTS_H */