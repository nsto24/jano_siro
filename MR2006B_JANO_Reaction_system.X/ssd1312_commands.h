/* 
 * File:                ssd1322_commands.h
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    23.09.2026 17:21 - Created file
 */


#ifndef SSD1312_COMMANDS_H
#define	SSD1312_COMMANDS_H

// Display command defines
#define SET_MEMORY_ADDR_MODE        0x20
#define SET_COLUMN_ADDR             0x21
#define SET_PAGE_ADDR               0x22

#define SET_DISP_START_LINE         0x40

#define SET_CONTRAST                0x81
#define CHARGE_PUMP_SETTING         0x8D

#define SEGMENT_REMAP_NORMAL        0xA0
#define SEGMENT_REMAP_REVERSE       0xA1

#define ENTIRE_DISPLAY_ON_RESUME    0xA4
#define ENTIRE_DISPLAY_ON_FORCE     0xA5

#define SET_NORMAL_DISPLAY          0xA6
#define SET_INVERT_DISPLAY          0xA7

#define SET_MULTIPLEX_RATIO         0xA8
#define SLEEP_MODE_ON               0xAE
#define SLEEP_MODE_OFF              0xAF

#define COM_SCAN_DIR_NORMAL         0xC0
#define COM_SCAN_DIR_REVERSE        0xC8

#define SET_DISP_OFFSET             0xD3
#define SET_DISPLAY_CLOCK_DIV       0xD5
#define SET_PRECHARGE_PERIOD        0xD9
#define SET_COM_PINS_HW_CONFIG      0xDA
#define SET_VCOMH_DESELECT          0xDB


#endif	/* SSD1312_COMMANDS_H */

