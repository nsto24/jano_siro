/* 
 * File:    fonts.h
 * Author:  Nicolas Stock
 *
 * Created on 23. September 2026, 19:51
 */

#ifndef FONTS_H
#define FONTS_H

// Font Structure Definition
typedef struct {
    const unsigned char* data;  // Pointer to the font array
    unsigned char width;        // Width of the character in pixels
    unsigned char height;       // Height of the character in pixels
} FontDef;

// Declarations for Font Data Arrays
extern const unsigned char Font5x7[];
extern const unsigned char Font11x18[];

// Declarations for Font Profiles
extern FontDef Font_5x7;
extern FontDef Font_11x18;

// Declarations for Icons
extern const unsigned char icon_bt[];
extern const unsigned char icon_speaker[];
extern const unsigned char icon_arrow[];
#endif    /* FONTS_H */