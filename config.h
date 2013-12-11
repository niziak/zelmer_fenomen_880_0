/* 
 * File:   config.h
 * Author: nizinski_w
 *
 * Created on 11 grudzień 2013, 10:01
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define KEY_DEBOUNCE_WAIT_MS        20          ///< time in ms, to confirm that key is still pressed
#define KEY_DEBOUNCE_WAIT           ((KEY_DEBOUNCE_WAIT_MS)/12)   ///< ISR works at 244Hz = every 4 ms x 3 columns =  12 ms.

#define EINT_EXPECTED_EVERY_MS      200         ///< if no EINT after this time - assume device overheat
#define EINT_EXPECTED_T0_CYCLES     ((EINT_EXPECTED_EVERY_MS)/4)


#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

