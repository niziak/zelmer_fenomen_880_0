/* 
 * File:   isr_ext.h
 * Author: nizinski_w
 *
 * Created on 10 grudzień 2013, 07:35
 */

#ifndef ISR_EXT_H
#define	ISR_EXT_H

#ifdef	__cplusplus
extern "C" {
#endif

extern inline void EINT_vIsr(void);
extern inline void EINT_vInit(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ISR_EXT_H */

