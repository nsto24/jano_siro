/* 
 * File:                ssd1322.h
 * Author:              Nicolas Stock
 * Comments:    
 * Revision history:    23.09.2026 17:14 - Created file
 */


#ifndef SSD1312_H
#define	SSD1312_H

#include "fonts.h"

// Hardware specifications
/*
 * RES      - Reset pin
 * DC       - Data/Command pin (0 = command , 1 = data)
 * CS       - Chip select (SPI)
 * SCL      - Bus clock (SPI)
 * SDA      - Bus data (SPI)
 */

// Hardware defines
#define SSD1312_PX_HEIGHT   64
#define SSD1312_PX_WIDTH    128

// Color defines (Monochrome)
#define BLACK      0x00
#define WHITE      0x01

//
#define DISPLAY_HEADBAR_WIDTH   11
#define MAX_ITEMS_PER_PAGE      5

// Functions
void Init_Display(void);
void Display_Clear(void);
void Display_Update(void);
void Display_Draw_Pixel(unsigned int x, unsigned int y, unsigned char color);
void Display_Draw_Char(unsigned int x, unsigned int y, char c, FontDef* font, unsigned char color);
void Display_Draw_String(unsigned int x, unsigned int y, char* str, FontDef* font, unsigned char color);
void Display_Draw_Icon(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char* icon, unsigned char color);
void Display_Draw_Headbar(void);
unsigned char getTextCentreXPos(const char* txt, const char charWidth);
void Display_Draw_Mode_Screen(const char* title, const char* instruction1, const char* instruction2);

#endif	/* SSD1312_H */

