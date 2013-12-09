
#include <xc.h>

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
 * 
 * LED Segments to port bits mapping (active LOW):
 * 
 * LED SEG:   dp     E   F   B   A   D   C   G
 * MCU PORT:  C7     B7  B6  B5  B4  B3  B2  B1
 *
 */

// 2 block seven segment display is used
// also two color LED is connected to the same pins

const unsigned char acDigitToSegMap[] =
{
    /*              EFBADCGx */
    /* 0 */     ~(0b11111100),             // 0
    /* 1 */     ~(0b00100100),             // 1
    /* 2 */     ~(0b10111010),             // 2
    /* 3 */     ~(0b00111110),             // 3
    /* 4 */     ~(0b01100110),             // 4
    /* 5 */     ~(0b01011110),             // 5
    /* 6 */     ~(0b11011110),             // 6
    /* 7 */     ~(0b00110100),             // 7
    /* 8 */     ~(0b11111110),             // 8
    /* 9 */     ~(0b01111110),             // 9

    /* E */     ~(0b11011010),             // 10
    /* r */     ~(0b10000010),             // 11
};




unsigned char acDispContent[2];  ///< 2 byte array to store current display content


void DISP_Refresh(void)
{
    PORTB = acDigitToSegMap[acDispContent[0]];
}