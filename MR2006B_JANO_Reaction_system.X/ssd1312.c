/* 
 * File:                ssd1312.c   
 * Author:              Nicolas Stock
 * Comments:            Frame buffer implementation for SSD1312 (Multi-Font Support)
 * Revision history:    13.10.2026
 */

#include "globals.h"
#include "ssd1312.h"
#include "ssd1312_commands.h"
#include "fonts.h"
#include "spi_functions.h"
#include "inputs.h"
#include "dfplayermini.h"
#include "hc05.h"
#include <string.h>

// Local Frame Buffer: 128 columns * 8 pages (8 pixels per page) = 1024 Bytes
unsigned char SSD1312_Buffer[1024];

void Init_Display(void) {
    // 1. Hardware Reset
    DISPLAY_RES = 0;                      
    __delay_ms(50);                      
    DISPLAY_RES = 1;                    
    __delay_ms(50);                      

    // 2. Lock the bus for a continuous command stream
    SPI_CS_DISPLAY = 0;                     
    DISPLAY_DC = 0;                         
    __delay_us(10);

    SPI_Write_CH1(SLEEP_MODE_ON);

    SPI_Write_CH1(SET_DISPLAY_CLOCK_DIV);
    SPI_Write_CH1(0x80);

    SPI_Write_CH1(SET_MULTIPLEX_RATIO);
    SPI_Write_CH1(0x3F);

    SPI_Write_CH1(SET_DISP_OFFSET);
    SPI_Write_CH1(0x00);

    SPI_Write_CH1(SET_DISP_START_LINE | 0x00);

    SPI_Write_CH1(CHARGE_PUMP_SETTING);
    SPI_Write_CH1(0x14); 

    SPI_Write_CH1(SET_MEMORY_ADDR_MODE);
    SPI_Write_CH1(0x00); 

    SPI_Write_CH1(SEGMENT_REMAP_REVERSE);
    SPI_Write_CH1(COM_SCAN_DIR_REVERSE);

    SPI_Write_CH1(SET_COM_PINS_HW_CONFIG);
    SPI_Write_CH1(0x12);

    SPI_Write_CH1(SET_CONTRAST);
    SPI_Write_CH1(0xCF);

    SPI_Write_CH1(SET_PRECHARGE_PERIOD);
    SPI_Write_CH1(0xF1);

    SPI_Write_CH1(SET_VCOMH_DESELECT);
    SPI_Write_CH1(0x40);

    SPI_Write_CH1(ENTIRE_DISPLAY_ON_RESUME);
    SPI_Write_CH1(SET_NORMAL_DISPLAY);
    SPI_Write_CH1(SLEEP_MODE_OFF);

    // 3. Unlock the bus
    SPI_CS_DISPLAY = 1;
    __delay_ms(100); // Give the charge pump time to stabilize
    
    // 4. Clear RAM and local buffer
    Display_Clear();
    Display_Update();
}

void Display_Clear(void) {
    // Clear the local buffer array
    for(unsigned int i = 0; i < 1024; i++) {
        SSD1312_Buffer[i] = 0x00;
    }
}

void Display_Update(void) {
    // Continuous 3-byte command for Column Address
    SPI_CS_DISPLAY = 0;
    DISPLAY_DC = 0;
    SPI_Write_CH1(SET_COLUMN_ADDR);
    SPI_Write_CH1(0);
    SPI_Write_CH1(SSD1312_PX_WIDTH - 1);
    SPI_CS_DISPLAY = 1;

    // Continuous 3-byte command for Page Address
    SPI_CS_DISPLAY = 0;
    DISPLAY_DC = 0;
    SPI_Write_CH1(SET_PAGE_ADDR);
    SPI_Write_CH1(0);
    SPI_Write_CH1((SSD1312_PX_HEIGHT / 8) - 1);
    SPI_CS_DISPLAY = 1;

    // Push the entire 1024-byte pixel buffer
    SPI_Write_Data_Array_Display(SSD1312_Buffer, 1024);
    FLAG_redraw_display = 0;
}

void Display_Draw_Pixel(unsigned int x, unsigned int y, unsigned char color) {
    // Prevent drawing outside bounds
    if (x >= SSD1312_PX_WIDTH || y >= SSD1312_PX_HEIGHT) {
        return; 
    }

    // Calculate buffer index (x + (page * 128))
    unsigned int index = x + ((y / 8) * SSD1312_PX_WIDTH);
    
    // Calculate which bit in the byte corresponds to the y coordinate
    unsigned char bit_mask = (unsigned char)(1 << (y % 8));

    if (color == WHITE) {
        SSD1312_Buffer[index] |= bit_mask;     // Set bit
    } else {
        SSD1312_Buffer[index] &= ~bit_mask;    // Clear bit
    }
}

void Display_Draw_Char(unsigned int x, unsigned int y, char c, FontDef* font, unsigned char color) {
    // Boundary check
    if ((x + font->width > SSD1312_PX_WIDTH) || (y + font->height > SSD1312_PX_HEIGHT)) {
        return; 
    }

    unsigned int char_index = 0;

    // 1. Standard Printable ASCII Mapping (ASCII 32 ' ' to 126 '~')
    if (c < 32 || c > 126) {
        return;
    }
    char_index = (unsigned int)(c - 32);

    // 2. Calculate how many vertical bytes make up one column of the font
    unsigned char bytes_per_column = (font->height + 7) / 8;
    
    // 3. Find character starting index in the array
    unsigned int font_index = char_index * font->width * bytes_per_column;

    // 4. Draw bit-by-bit into the frame buffer
    for (unsigned int col = 0; col < font->width; col++) {
        for (unsigned int byte_idx = 0; byte_idx < bytes_per_column; byte_idx++) {
            
            // Extract the specific byte for this vertical column segment
            unsigned char font_data = font->data[font_index + (col * bytes_per_column) + byte_idx];
            
            // Iterate through the 8 bits of the extracted byte
            for (unsigned int bit = 0; bit < 8; bit++) {
                unsigned int row = (byte_idx * 8) + bit;
                
                if (row >= font->height) break; // Prevent drawing garbage beyond font height
                
                if (font_data & (1 << bit)) {
                    Display_Draw_Pixel(x + col, y + row, color);
                } else {
                    // Draw background
                    Display_Draw_Pixel(x + col, y + row, color == WHITE ? BLACK : WHITE); 
                }
            }
        }
    }
}

void Display_Draw_String(unsigned int x, unsigned int y, char* str, FontDef* font, unsigned char color) {
    while (*str) {
        Display_Draw_Char(x, y, *str, font, color);
        x += (font->width + 1); // 1 pixel gap between characters
        str++; 
    }
}

void Display_Draw_Icon(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char* icon, unsigned char color) {
    unsigned char bytes_per_column = (height + 7) / 8;
    
    for (unsigned int col = 0; col < width; col++) {
        for (unsigned int byte_idx = 0; byte_idx < bytes_per_column; byte_idx++) {
            unsigned char icon_data = icon[(col * bytes_per_column) + byte_idx];
            
            for (unsigned int bit = 0; bit < 8; bit++) {
                unsigned int row = (byte_idx * 8) + bit;
                if (row >= height) break; 
                
                if (icon_data & (1 << bit)) {
                    Display_Draw_Pixel(x + col, y + row, color);
                } else {
                    Display_Draw_Pixel(x + col, y + row, color == WHITE ? BLACK : WHITE); // Background
                }
            }
        }
    }
}

void Display_Draw_Headbar(void) {
    unsigned char dist_edge = 2;        // 2px right screen margin
    const unsigned char icon_gap = 4;   // 4px gap

    // 1. Clear the right-hand icon zone
    for (unsigned int x = 92; x < SSD1312_PX_WIDTH; x++) {
        for (unsigned int y = 0; y < DISPLAY_HEADBAR_WIDTH; y++) {
            Display_Draw_Pixel(x, y, BLACK);
        }
    }

    // 2. Draw horizontal separator at Y = DISPLAY_HEADBAR_WIDTH
    for (unsigned int x = 0; x < SSD1312_PX_WIDTH; x++) {
        Display_Draw_Pixel(x, DISPLAY_HEADBAR_WIDTH, WHITE);
    }

    // 3. Draw static title text
    Display_Draw_String(0, 2, "Hola Rosita!", &Font_5x7, WHITE);

    // 4. Draw Bluetooth status icon (Right-aligned)
    if (HC05_STATE) { // BT connected
        dist_edge += ICON_BT_CONN_WIDTH;
        Display_Draw_Icon(SSD1312_PX_WIDTH - dist_edge, 1, ICON_BT_CONN_WIDTH, ICON_HEIGHT, icon_bt_connected, WHITE);
    } else {          // BT disconnected
        dist_edge += ICON_BT_DISC_WIDTH;
        Display_Draw_Icon(SSD1312_PX_WIDTH - dist_edge, 1, ICON_BT_DISC_WIDTH, ICON_HEIGHT, icon_bt_disconnected, WHITE);
    }

    dist_edge += icon_gap;

    // 5. Draw speaker / volume icon
    if (isVolumeZero()) {
        Display_Draw_Icon(SSD1312_PX_WIDTH - (dist_edge + ICON_VOL_MUTE_WIDTH), 1, ICON_VOL_MUTE_WIDTH, ICON_HEIGHT, icon_vol_mute, WHITE);
    }
    else if (isVolumeLow()) {
        Display_Draw_Icon(SSD1312_PX_WIDTH - (dist_edge + ICON_VOL_LOW_WIDTH), 1, ICON_VOL_LOW_WIDTH, ICON_HEIGHT, icon_vol_low, WHITE);
    }
    else if (isVolumeMid()) {
        Display_Draw_Icon(SSD1312_PX_WIDTH - (dist_edge + ICON_VOL_MID_WIDTH), 1, ICON_VOL_MID_WIDTH, ICON_HEIGHT, icon_vol_mid, WHITE);
    }
    else if (isVolumeHigh()) {
        Display_Draw_Icon(SSD1312_PX_WIDTH - (dist_edge + ICON_VOL_HIGH_WIDTH), 1, ICON_VOL_HIGH_WIDTH, ICON_HEIGHT, icon_vol_high, WHITE);
    }
    
    // 6. Set global redraw flag
    if (hasBtStateChanged() || hasDfpStateChanged() || hasVolumeChanged()) {
        FLAG_redraw_display = 1;
    }
}

unsigned char getTextCentreXPos(const char* txt, const char charWidth){
    unsigned int txtLen = strlen(txt);
    unsigned int txtWidth = (txtLen > 0) ? ((txtLen * (charWidth + 1u)) - 1u) : 0;
    unsigned int txtX = (txtWidth < SSD1312_PX_WIDTH) ? ((SSD1312_PX_WIDTH - txtWidth) / 2) : 0;
    return (unsigned char)txtX;
}

void Display_Draw_Mode_Screen(const char* title, const char* instruction1, const char* instruction2){
    unsigned char xPos = 0;
    unsigned char yPos = DISPLAY_HEADBAR_WIDTH + 2;
    const unsigned char ySep = 3;
    
    // Clear previous menu content and restore top status bar
    Display_Clear();
    Display_Draw_Headbar();
    
    // Title uses bigger font 11x18
    xPos = getTextCentreXPos(title, 11);
    Display_Draw_String(xPos, yPos, title, &Font_11x18, WHITE);
    yPos += 18+ySep;
    
    xPos = getTextCentreXPos(instruction1, 5);
    Display_Draw_String(xPos, yPos, instruction1, &Font_5x7, WHITE);
    yPos += 7+ySep;
    
    xPos = getTextCentreXPos(instruction2, 5);
    Display_Draw_String(xPos, yPos, instruction2, &Font_5x7, WHITE);
    
    FLAG_redraw_display = 1;
}