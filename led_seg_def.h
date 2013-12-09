/* 
 * File:   led_seg_def.h
 * Author: nizinski_w
 *
 * Created on 9 grudzień 2013, 08:37
 */



/**
 * Part: TOD-2281BG-N
 * Manufacturer: OASISTEK
 * 2 digits common anode seven segments LED, green color, 2.2V forward voltage
 *
 *
 *      --A--       Bit:  7 6 5 4 3 2 1 0
 *     |     |
 *     F     B      LED PIN:  5   7   8   3   2   1   10   4
 *     +--G--+      LED SEG:  dp  A   B   C   D   E   F    G
 *     E     C      MCU PIN:  18  25  25  23  24  28  27   22
 *     |     |      MCU PORT: C7  B4  B5  B2  B3  B7  B6   B1
 *      --D--  dp
 *
 *                  Anode Digit 1: PIN 9    - MCU PIN 16 (RC5)
 *                  Anode Digit 2: PIN 6    - MCU PIN 15 (RC4)
 *
 *  __      __   __        __   __  __    __   __
 * |  |  |  __|  __| |__| |__  |__    |  |__| |__|
 * |__|  | |__   __|    |  __| |__|   |  |__|  __|
 *
 *   __                __                   __    __
 *  |__    __         |__| |__    __   __| |__|  |__
 *  |__   |           |  | |__|  |__  |__| |__   |
 *
 *   __
 *  |__|
 *  |
 *
 * 
 * LED Segments to port bits mapping (active LOW):
 *
 * LED SEG:   dp     E   F   B   A   D   C   G
 * MCU PORT:  C7     B7  B6  B5  B4  B3  B2  B1
 *
 */
    
#ifndef SEG_CHAR_DEF
    #warning "SEG_CHAR_DEF macro not defined";
    #define SEG_CHAR_DEF(enum,byte)
#endif
    
/* seven segments display characters definitions             */
/*              ENUM_VALUE          SEGMENTS TO TURN ON      */
/*                                  EFBADCGx                 */
   
SEG_CHAR_DEF(   SEG_0,           (0b11111100)       )
SEG_CHAR_DEF(   SEG_1,           (0b00100100)       )
SEG_CHAR_DEF(   SEG_2,           (0b10111010)       )
SEG_CHAR_DEF(   SEG_3,           (0b00111110)       )
SEG_CHAR_DEF(   SEG_4,           (0b01100110)       )
SEG_CHAR_DEF(   SEG_5,           (0b01011110)       )
SEG_CHAR_DEF(   SEG_6,           (0b11011110)       )
SEG_CHAR_DEF(   SEG_7,           (0b00110100)       )
SEG_CHAR_DEF(   SEG_8,           (0b11111110)       )
SEG_CHAR_DEF(   SEG_9,           (0b01111110)       )
/*                                  EFBADCGx                 */
SEG_CHAR_DEF(   SEG_A,           (0b11110110)       )   /* A */
SEG_CHAR_DEF(   SEG_B,           (0b11001110)       )   /* b */
SEG_CHAR_DEF(   SEG_C,           (0b11011000)       )   /* c */
SEG_CHAR_DEF(   SEG_D,           (0b10101110)       )   /* d */
SEG_CHAR_DEF(   SEG_E,           (0b11011010)       )   /* E */
SEG_CHAR_DEF(   SEG_F,           (0b11010010)       )   /* F */
/*                                  EFBADCGx                 */
SEG_CHAR_DEF(   SEG_R,           (0b10000010)       )   /* r */
SEG_CHAR_DEF(   SEG_P,           (0b11110010)       )   /* P */
/*                                  EFBADCGx                 */
SEG_CHAR_DEF(   SEG_NONE,        (0b00000000)       )   /* display off */


