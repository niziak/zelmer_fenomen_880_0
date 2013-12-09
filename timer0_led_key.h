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

extern void T0_vIsr(void);
extern void T0_vInit(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER0_LED_H */

