/* 
 * File:                dfplayermini_commands.h   
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    08.10.2026 18:51 - Created file
 */

#ifndef DFPLAYERMINICOMMANDS_H
#define	DFPLAYERMINICOMMANDS_H

/*  Structure of data packet send over UART:
    $S          Start byte 0x7E
    Ver.        Version 0xFF by default
    Length      Number of byte from version info to Check_LSB, typically 0x06 (checksum not counted)
    CMD         Command Code
    Feedback    0x01: Need feedback--send confirmation back to MCU; 0x00: No need feedback
    Para_MSB    Most significant byte of parameter
    Para_LSB    Least significant byte of parameter
    Check_MSB   Most significant byte of checksum
    Check_LSB   Least significant byte of checksum
    $O          End byte 0xEF
  
    Calculation of checksum:
    Checksum (2 bytes) = 0xFFFF?(Ver.+Length+CMD+Feedback+Para_MSB+Para_LSB)+1
 */

// Message prefix and suffix
#define DFP_START_BYTE                                                      0x7E
#define DFP_VERSION                                                         0xFF
#define DFP_LENGTH                                                          0x06
#define DFP_FEEDBACK                                                        0x01
#define DFP_NO_FEEDBACK                                                     0x00
#define DFP_END_BYTE                                                        0xEF

// Serial commands - Control commands
#define DFP_PLAY_NEXT                                                       0x01
#define DFP_PLAY_PREVIOUS                                                   0x02
#define DFP_SPECIFY_PLAYBACK_OF_A_TRACK                                     0x03
#define DFP_INCREASE_VOLUME                                                 0x04
#define DFP_DECREASE_VOLUME                                                 0x05
#define DFP_SPECIFY_VOLUME                                                  0x06
#define DFP_SPECIFY_EQ                                                      0x07
#define DFP_SPECIFY_SINGLE_REPEAT_PLAYBACK                                  0x08
#define DFP_SPECIFY_PLAYBACK_OF_A_DEVICE                                    0x09
#define DFP_SET_SLEEP                                                       0x0A
#define DFP_RESET                                                           0x0C
#define DFP_PLAY                                                            0x0D
#define DFP_PAUSE                                                           0x0E
#define DFP_SPECIFY_PLAYBACK_A_TRACK_IN_A_FOLDER                            0x0F
#define DFP_AUDIO_AMPLIFICATION_SETTING                                     0x10
#define DFP_SET_ALL_REPEAT_PLAYBACK                                         0x11
#define DFP_SPECIFY_PLAYBACK_OF_FOLDER_NAMED_MP3                            0x12
#define DFP_INSERT_AN_ADVERTISEMENT                                         0x13
#define DFP_SPECIFY_PLAYBACK_A_TRACK_IN_A_FOLDER_THAT_SUPPORTS_3000_TRACKS  0x14
#define DFP_STOP_PLAYING_ADVERTISEMENT_GO_BACK_PLAY_THE_MUSIC_INTERRUPTED   0x15
#define DFP_STOP                                                            0x16
#define DFP_SPECIFY_REPEAT_PLAYBACK_OF_A_FOLDER                             0x17
#define DFP_SET_RANDOM_PLAYBACK                                             0x18
#define DFP_SET_REPEAT_PLAYBACK_OF_CURRENT_TRACK                            0x19
#define DFP_SET_DAC                                                         0x1A

// Serial commands - Query commands
#define DFP_QUERY_CURRENT_ONLINE_STORAGE_DEVICE                             0x3F
#define DFP_RETURN_ERROR_DATA                                               0x40
#define DFP_REPORTS_FEEDBACK                                                0x41
#define DFP_QUERY_CURRENT_STATUS                                            0x42
#define DFP_QUERY_CURRENT_VOLUME                                            0x43
#define DFP_QUERY_CURRENT_EQ                                                0x44
#define DFP_QUERY_NUMBER_OF_TRACKS_IN_THE_ROOT_OF_USB_FLASH_DRIVE           0x47
#define DFP_QUERY_NUMBER_OF_TRACKS_IN_THE_ROOT_OF_MICRO_SD_CARD             0x48
#define DFP_QUERY_CURRENT_TRACK_IN_THE_USB_FLASH_DRIVE                      0x4B
#define DFP_QUERY_CURRENT_TRACK_IN_THE_MICRO_SD_CARD                        0x4C
#define DFP_QUERY_NUMBER_OF_TRACKS_IN_A_FOLDER                              0x4E
#define DFP_QUERY_NUMBER_OF_FOLDERS_IN_THE_CURRENT_STORAGE_DEVICE           0x4F

#define DFP_TF_CARD_ONLINE                                                  0x02

#endif	/* DFPLAYERMINICOMMANDS_H */

