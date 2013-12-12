/* 
 * File:   timer0_led.h
 * Author: nizinski_w
 *
 * Created on 9 grudzień 2013, 10:44
 */

#ifndef TIMER0_LED_H
#define	TIMER0_LED_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
    COL_1ST_DIGIT = 0,      ///< first digit and key column [-] and [PULSE]
    COL_2ND_DIGIT = 1,      ///< second digit and key column [+] and [AUTO]
    COL_LED       = 2,      ///< bicolor LED and key column [TURBO] and [ON/OFF]
} SCAN_COL_DEF;


extern inline void T0_vIsr(void);
extern inline void T0_vInit(void);

extern volatile unsigned char ucEINTGap;
extern volatile SCAN_COL_DEF eSelectedColumn;

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER0_LED_H */

