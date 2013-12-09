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

#define DSP_EN_1ST  { TRISCbits.TRISC4=0;  TRISCbits.TRISC5=0; PORTCbits.RC4 = 1; PORTCbits.RC5 = 0;}
#define DSP_EN_2ND  { TRISCbits.TRISC4=0;  TRISCbits.TRISC5=0; PORTCbits.RC4 = 0; PORTCbits.RC5 = 1;}

extern unsigned char acDispContent[2];  ///< 2 byte array to store current display content
extern const unsigned char acDigitToSegMap[];

#ifdef	__cplusplus
}
#endif

#endif	/* LED_SEG_H */

