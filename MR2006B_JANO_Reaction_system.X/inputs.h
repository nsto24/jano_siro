/* 
 * File:                inputs.h
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    31.08.2026 17:09 - Created file
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

// Global variable definition
typedef struct{
    int x;
    int y;
} joystick_t;

// Joystick Functions
/*
 * The Joystick's axis are placed as:
 * X: vertical axis, referred to in its extremes as UP and DOWN
 * Y: horizontal axis, referred to in its extremes as LEFT and RIGHT
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
char isJoyStickLeft(void);

/**
    <p><b>Function prototype: char isJoyStickRight(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "RIGHT" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
char isJoyStickRight(void);

/**
    <p><b>Function prototype: char isJoyStickUp(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "UP" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
char isJoyStickUp(void);

/**
    <p><b>Function prototype: char isJoyStickDown(void){}</b></p>
    <p><b>Summary: Checks Joystick X and Y axis and states position "DOWN" reached</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
char isJoyStickDown(void);

/**
    <p><b>Function prototype: char isJoyStickPressed(void){}</b></p>
    <p><b>Summary: Checks if Joystick is pressed down center</b></p>
    <p><b>Description: Reads values, evalues and returns bool statement</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: char</b></p>
 */
char isJoyStickPressed(void);

/**
    <p><b>Function prototype: float JoyStickAxisXpercent(void){}</b></p>
    <p><b>Summary: Asses X axis value of Joystick in percent</b></p>
    <p><b>Description: From left=0.0 to right=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float JoyStickAxisXpercent(void);

/**
    <p><b>Function prototype: float JoyStickAxisYpercent(void){}</b></p>
    <p><b>Summary: Asses Y axis value of Joystick in percent</b></p>
    <p><b>Description: From down=0.0 to up=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float JoyStickAxisYpercent(void);

/**
    <p><b>Function prototype: float JoyStickPitchAxisXpercent(void){}</b></p>
    <p><b>Summary: Assesses X axis value of Joystick's pitch in percent</b></p>
    <p><b>Description: From middle=0.0 to down=-1.0 and up=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float JoyStickPitchAxisXpercent(void);

/**
    <p><b>Function prototype: float JoyStickPitchAxisYpercent(void){}</b></p>
    <p><b>Summary: Assesses Y axis value of Joystick's pitch in percent</b></p>
    <p><b>Description: From middle=0.0 to left=-1.0 and right=1.0</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: float</b></p>
 */
float JoyStickPitchAxisYpercent(void);


#endif	/* INPUTS_H */