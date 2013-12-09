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

#define DSP_EN_1ST  { PORTCbits.RC4 = 1; PORTCbits.RC5 = 0; PORTCbits.RC0 = 1; }
#define DSP_EN_2ND  { PORTCbits.RC4 = 0; PORTCbits.RC5 = 1; PORTCbits.RC0 = 1; }
#define DSP_EN_LED  { PORTCbits.RC4 = 1; PORTCbits.RC5 = 1; PORTCbits.RC0 = 0; }
#define DSP_DISABLE { PORTCbits.RC4 = 1; PORTCbits.RC5 = 1; PORTCbits.RC0 = 1; }

extern volatile SEG_CHAR_ID_DEF acDispContent[2];  ///< 2 byte array to store current display content

extern const unsigned char acDigitToSegMap[];

#ifdef	__cplusplus
}
#endif

#endif	/* LED_SEG_H */

