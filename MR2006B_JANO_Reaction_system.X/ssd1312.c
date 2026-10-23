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

// Local Frame Buffer: 128 columns * 8 pages (8 pixels per page) = 1024 Bytes
unsigned char SSD1312_Buffer[1024];

void Init_Display(void) {
    // 1. Hardware Reset
    DISPLAY_RES = 0;                     
    __delay_ms(10);                      
    DISPLAY_RES = 1;                     
    __delay_ms(50);                      

    // 2. Initialization Sequence
    SPI_Write_Command_Display(SLEEP_MODE_ON);

    SPI_Write_Command_Display(SET_DISPLAY_CLOCK_DIV);
    SPI_Write_Data_Display(0x80);

    SPI_Write_Command_Display(SET_MULTIPLEX_RATIO);
    SPI_Write_Data_Display(0x3F);        // 64-1

    SPI_Write_Command_Display(SET_DISP_OFFSET);
    SPI_Write_Data_Display(0x00);

    SPI_Write_Command_Display(SET_DISP_START_LINE | 0x00);

    SPI_Write_Command_Display(CHARGE_PUMP_SETTING);
    SPI_Write_Data_Display(0x14);        // Enable charge pump

    SPI_Write_Command_Display(SET_MEMORY_ADDR_MODE);
    SPI_Write_Data_Display(0x00);        // Horizontal Addressing Mode

    SPI_Write_Command_Display(SEGMENT_REMAP_REVERSE);
    SPI_Write_Command_Display(COM_SCAN_DIR_REVERSE);

    SPI_Write_Command_Display(SET_COM_PINS_HW_CONFIG);
    SPI_Write_Data_Display(0x12);

    SPI_Write_Command_Display(SET_CONTRAST);
    SPI_Write_Data_Display(0xCF);

    SPI_Write_Command_Display(SET_PRECHARGE_PERIOD);
    SPI_Write_Data_Display(0xF1);

    SPI_Write_Command_Display(SET_VCOMH_DESELECT);
    SPI_Write_Data_Display(0x40);

    SPI_Write_Command_Display(ENTIRE_DISPLAY_ON_RESUME);
    SPI_Write_Command_Display(SET_NORMAL_DISPLAY);
    
    // 3. Clear RAM and local buffer
    Display_Clear();
    Display_Update();

    // 4. Power On
    SPI_Write_Command_Display(SLEEP_MODE_OFF);
}

void Display_Clear(void) {
    // Clear the local buffer array
    for(unsigned int i = 0; i < 1024; i++) {
        SSD1312_Buffer[i] = 0x00;
    }
}

void Display_Update(void) {
    // Set column window (0 to 127)
    SPI_Write_Command_Display(SET_COLUMN_ADDR);
    SPI_Write_Data_Display(0);
    SPI_Write_Data_Display(SSD1312_PX_WIDTH - 1);

    // Set page window (0 to 7)
    SPI_Write_Command_Display(SET_PAGE_ADDR);
    SPI_Write_Data_Display(0);
    SPI_Write_Data_Display((SSD1312_PX_HEIGHT / 8) - 1);

    // Push the entire 1024-byte buffer to the OLED in one continuous transmission
    SPI_Write_Data_Array_Display(SSD1312_Buffer, 1024);
}

void Display_Draw_Pixel(unsigned int x, unsigned int y, unsigned char color) {
    // Prevent drawing outside bounds
    if (x >= SSD1312_PX_WIDTH || y >= SSD1312_PX_HEIGHT) {
        return; 
    }

    // Calculate buffer index (x + (page * 128))
    unsigned int index = x + ((y / 8) * SSD1312_PX_WIDTH);
    
    // Calculate which bit in the byte corresponds to the y coordinate
    unsigned char bit_mask = 1 << (y % 8);

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

    // Unprintable character check (Assumes ASCII 32 to 126)
    if (c < 32 || c > 126) return; 

    // Calculate how many vertical bytes make up one column of the font
    unsigned char bytes_per_column = (font->height + 7) / 8;
    
    // Find character starting index in the array
    unsigned int font_index = (c - 32) * font->width * bytes_per_column;

    // Draw bit-by-bit into the frame buffer
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
                    Display_Draw_Pixel(x + col, y + row, BLACK); // Draw background
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