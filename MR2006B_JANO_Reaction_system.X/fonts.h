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

// Declarations Icons (all 9 pixels high)
#define ICON_HEIGHT             9

#define ICON_VOL_HIGH_WIDTH     18
#define ICON_VOL_MID_WIDTH      14
#define ICON_VOL_LOW_WIDTH      11
#define ICON_VOL_MUTE_WIDTH     14
#define ICON_VOL_UP_WIDTH       14
#define ICON_VOL_DOWN_WIDTH     14
#define ICON_BT_CONN_WIDTH      5
#define ICON_BT_DISC_WIDTH      11

extern const unsigned char icon_vol_high[];
extern const unsigned char icon_vol_mid[];
extern const unsigned char icon_vol_low[];
extern const unsigned char icon_vol_mute[];
extern const unsigned char icon_vol_up[];
extern const unsigned char icon_vol_down[];
extern const unsigned char icon_bt_connected[];
extern const unsigned char icon_bt_disconnected[];
extern const unsigned char icon_arrow[];
#endif    /* FONTS_H */