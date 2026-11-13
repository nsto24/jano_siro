/*
 * File:                dfplayermini.c
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    08.10.2026, 19:04
 */

// #include
#include "globals.h"
#include "dfplayermini_commands.h"
#include "uart.h"
#include "dfplayermini.h"

// Functions

uint16_t DFP_calc_checksum(unsigned char cmd, unsigned char feedback, uint16_t parameter){
    // Checksum (2 bytes) = 0xFFFF?(Ver.+Length+CMD+Feedback+Para_MSB+Para_LSB)+1
    return 0xFFFF - (DFP_VERSION + DFP_LENGTH + cmd + feedback + ((parameter & 0xFF00) >> 8) + (parameter & 0x00FF)) + 1;
}

void DFP_sendCommand(unsigned char cmd, unsigned char feedback, uint16_t parameter){
    unsigned char DFP_msg[10];
    uint16_t checksum = 0x0000;
    
    if(feedback != 0){ feedback = 0x01; }
    else{ feedback = 0x00; }
    
    DFP_msg[0] = DFP_START_BYTE;                                // 0x7E
    DFP_msg[1] = DFP_VERSION;                                   // 0xFF
    DFP_msg[2] = DFP_LENGTH;                                    // 0x06
    DFP_msg[3] = cmd;                                           // command defined as byte
    DFP_msg[4] = feedback;                                      // feedback: 0x01, no feedback: 0x00
    DFP_msg[5] = (parameter & 0xFF00) >> 8;                     // MSB of parameter
    DFP_msg[6] = (parameter & 0x00FF);                          // LSB of parameter
    checksum   = DFP_calc_checksum(cmd, feedback, parameter);   // Generate checksum of 16bit
    DFP_msg[7] = (checksum & 0xFF00) >> 8;                      // MSB of checksum
    DFP_msg[8] = (checksum & 0x00FF);                           // LSB of checksum  
    DFP_msg[9] = DFP_END_BYTE;                                  // 0xEF                               
    
    for(unsigned char i = 0; i < sizeof(DFP_msg); i++){
        UART_WriteChar2(DFP_msg[i]);
    }
}

void DFP_specificVolume(unsigned char vol){
    if(vol > 30){vol = 30;}
    DFP_sendCommand(DFP_SPECIFY_VOLUME, DFP_NO_FEEDBACK,(uint16_t)vol);
}

void DFP_increaseVolume(){
    DFP_sendCommand(DFP_INCREASE_VOLUME, DFP_NO_FEEDBACK, 0x0000);
}

void DFP_decreaseVolume(){
    DFP_sendCommand(DFP_DECREASE_VOLUME, DFP_NO_FEEDBACK, 0x0000);
}

void DFP_play_trackMP3(unsigned int num){
    DFP_sendCommand(DFP_SPECIFY_PLAYBACK_OF_FOLDER_NAMED_MP3, DFP_NO_FEEDBACK, (uint16_t)num);
}

void DFP_pause(){
    DFP_sendCommand(DFP_PAUSE, DFP_NO_FEEDBACK, 0x0000);
}

void DFP_play(){
    DFP_sendCommand(DFP_PLAY, DFP_NO_FEEDBACK, 0x0000);
}

void DFP_reset(){
    DFP_sendCommand(DFP_RESET, DFP_NO_FEEDBACK, 0x0001);
}

void DFP_enableDAC(){   // Enables output DAC, for audio jack
    DFP_sendCommand(DFP_SET_DAC, DFP_NO_FEEDBACK, 0x0000);
}

void DFP_disableDAC(){  // Disables output DAC, for audio jack
    DFP_sendCommand(DFP_SET_DAC, DFP_NO_FEEDBACK, 0x0001);
}

void DFP_set_EQ(dfp_eq_t eq){
    DFP_sendCommand(DFP_SPECIFY_EQ, DFP_NO_FEEDBACK, (unsigned char)eq);
}

void Init_DFP(void){
    // Startup delay
    SPEAKER_MUTE = 1;   // Muting speaker to prevent popping noises
    for(unsigned int i = 0; i < 300; i++){ __delay_ms(10); }   // ~3 s
    // Select SD Card as source
    DFP_sendCommand(DFP_SPECIFY_PLAYBACK_OF_A_DEVICE, DFP_NO_FEEDBACK, DFP_TF_CARD_ONLINE);
    __delay_ms(200);
    // Initialize volume
    DFP_specificVolume(15);
    __delay_ms(200);
    // Setting EQ
    DFP_set_EQ(DFP_EQ_NORMAL);
    __delay_ms(200);
    // Enabling Speaker/Audio Jack
    DFP_enableDAC();
    __delay_ms(200);
}