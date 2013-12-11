/* 
 * File:   led_seg.h
 * Author: nizinski_w
 *
 * Created on 6 grudzień 2013, 08:59
 */

#ifndef LED_SEG_H
#define	LED_SEG_H


#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
    #define SEG_CHAR_DEF(enum,segments)    enum,
        #include "led_seg_def.h"
    #undef SEG_CHAR_DEF
    SEG_LAST,   ///< enum value which represents number of defined characters
} SEG_CHAR_ID_DEF;

typedef struct
{
    unsigned int    bDec1       : 1;     ///< decimal point of first display
    unsigned int    bDec2       : 1;     ///< decimal point of second display
    unsigned int    bLedRed     : 1;     ///< Red LED
    unsigned int    bLedGreen   : 1;     ///< Green LED
    unsigned int    bLedBlink   : 1;     ///< bicolor led is blinking
} LED_OTHER_DEF;

#define DSP_EN_1ST  { PORTCbits.RC4 = 1; PORTCbits.RC5 = 0; PORTCbits.RC0 = 1; }
#define DSP_EN_2ND  { PORTCbits.RC4 = 0; PORTCbits.RC5 = 1; PORTCbits.RC0 = 1; }
#define DSP_EN_LED  { PORTCbits.RC4 = 1; PORTCbits.RC5 = 1; PORTCbits.RC0 = 0; }
#define DSP_DISABLE { PORTCbits.RC4 = 1; PORTCbits.RC5 = 1; PORTCbits.RC0 = 1; }

#define DSP_EN_DEC  { PORTCbits.RC7 = 0; }
#define DSP_DIS_DEC { PORTCbits.RC7 = 1; }

//                    76543210
#define LED_NONE    0b00000000
#define LED_RED     0b01000000      // RB6
#define LED_GREEN   0b00001000      // RB3
#define LED_ORANGE  0b01001000      // RB6+RB3


extern volatile SEG_CHAR_ID_DEF acDispContent[2];  ///< 2 byte array to store current display content
extern volatile LED_OTHER_DEF   stDisp;            ///< bit field with led state and decimal point state
extern const unsigned char acDigitToSegMap[];
extern void LED_Disp(SEG_CHAR_ID_DEF eChar1, SEG_CHAR_ID_DEF eChar2);
extern void LED_DispHex (unsigned char ucValue);

#ifdef	__cplusplus
}
#endif

#endif	/* LED_SEG_H */

