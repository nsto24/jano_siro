/*
 * File:                dfplayermini.h
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    05.11.2026, 07:15
 */

//
typedef enum{
    DFP_EQ_NORMAL = 0,
    DFP_EQ_POP    = 1,
    DFP_EQ_ROCK   = 2,
    DFP_EQ_JAZZ   = 3,
    DFP_EQ_CLASSIC= 4,
    DFP_EQ_BASS   = 5
} dfp_eq_t;

// Functions

uint16_t DFP_calc_checksum(unsigned char cmd, unsigned char feedback, uint16_t parameter);

void DFP_sendCommand(unsigned char cmd, unsigned char feedback, uint16_t parameter);

void DFP_specificVolume(unsigned char vol);

void DFP_increaseVolume();

void DFP_decreaseVolume();

void DFP_play_trackMP3(unsigned int num);

void DFP_pause();

void DFP_play();

void DFP_reset();

void DFP_playNext();

void DFP_playPrevious();

void DFP_enableDAC();

void DFP_disableDAC();

void DFP_set_EQ(dfp_eq_t eq);

void Init_DFP(void);