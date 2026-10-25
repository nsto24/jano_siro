/*
 * File:                adc_functions.c
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    17.08.2026, 16:12
 */

#include "globals.h"
#include "adc_functions.h"

/*
 * ADCON0:  controls the operation of the ADC module
 * ADCON1:  configures port pins
 * ADCON2:  configures A/D clock, source, programmed acquisition time and justification    
 */

void Adc_SetChannel(unsigned char c){
    if(c >= 0 && c <= 27){
        ADCON0 = ADCON0 & 0b11000001;   // Clears bits 2,3,4,5,6
        ADCON0 = (unsigned char)(ADCON0 | (c << 2));     // Moves channel bits and ORs registry
        
        __delay_us(20);                 // Delay to await Channel switch
    }
}

void Adc_Enable(){
    ADCON0 = ADCON0 | 0b00000001;       // Sets bit0 to 1
}

void Adc_Disable(){
    ADCON0 = ADCON0 & 0b11111110;       // Clears bit 0
}

void Adc_ActivateVrefVoltage(){
    ADCON1 = ADCON1 | 0b00010000;       // Sets bit 4 (Vref+/- in use)
    __delay_us(50);
}

void Adc_ActivateInternalVoltage(){
    ADCON1 = ADCON1 & 0b11101111;       // Resets bit 4 (internal in use)
    __delay_us(50);
}

void Adc_StartConversion(){
    ADCON0 = ADCON0 | 0b00000010;       // Sets bit1 to 1 (gets cleared automatically by hardware)
}

int Adc_isConverting(){
    return ADCON0 & 0b00000010;         // returns 1 if GO-bit still 1
}

int Adc_ReadValue(){
    Adc_StartConversion();              // Starts ADC Conversion
    while (Adc_isConverting());         // Awaits finish of Conversion
    return ((ADRESH << 8) + ADRESL);    // Joins result int 16bit INT
}
