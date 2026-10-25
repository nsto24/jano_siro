/* 
 * File:                adc_functions.h   
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    31.08.2026 17:14 - Created file
 */

// Guard condition so that contents are not included more than once. 
#ifndef ADC_FUNCTIONS_H
#define	ADC_FUNCTIONS_H

/**
    <p><b>Function prototype: void Adc_SetChannel(unsigned char c){}</b></p>
    <p><b>Summary: Sets analog channel to be read</b></p>
    <p><b>Description:</b></p>
    <p><b>Parameters: unsigned char c</b></p>
    <p><b>Returns: void</b></p>
 */
void Adc_SetChannel(unsigned char c);

/**
    <p><b>Function prototype: void Adc_Enable(){}</b></p>
    <p><b>Summary: Enables ADC</b></p>
    <p><b>Description: Via ADON bit</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: void</b></p>
 */
void Adc_Enable(void);

/**
    <p><b>Function prototype: void Adc_Disable(){}</b></p>
    <p><b>Summary: Disables ADC</b></p>
    <p><b>Description: Via ADON bit</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: void</b></p>
 */
void Adc_Disable(void);

/**
    <p><b>Function prototype: void Adc_ActivateVrefVoltage(){}</b></p>
    <p><b>Summary: Enables use of Vref+/- for conversion</b></p>
    <p><b>Description: Via VDFG1/0 bits</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: void</b></p>
 */
void Adc_ActivateVrefVoltage(void);

/**
    <p><b>Function prototype: void Adc_ActivateInternalVoltage(){}</b></p>
    <p><b>Summary: Enables use of internal voltage for conversion</b></p>
    <p><b>Description: Via VDFG1/0 bits</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: void</b></p>
 */
void Adc_ActivateInternalVoltage(void);

/**
    <p><b>Function prototype: void Adc_StartConversion(){}</b></p>
    <p><b>Summary: Starts ADC Conversion</b></p>
    <p><b>Description: Via GO/DONE bit</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: void</b></p>
 */
void Adc_StartConversion(void);

/**
    <p><b>Function prototype: void Adc_isConverting(){}</b></p>
    <p><b>Summary: Returns true if ADC is converting</b></p>
    <p><b>Description: </b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: int</b></p>
 */
int Adc_isConverting(void);

/**
    <p><b>Function prototype: void Adc_ReadValue(){}</b></p>
    <p><b>Summary: Reads Value from pin</b></p>
    <p><b>Description: Starts conversion, awaits finish and returns value</b></p>
    <p><b>Parameters:</b></p>
    <p><b>Returns: int</b></p>
 */
int Adc_ReadValue(void);

#endif	/* ADC_FUNCTIONS_H */