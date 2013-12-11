/* 
 * File:   ccp1_engine_rpm.h
 * Author: nizinski_w
 *
 * Created on 10 grudzień 2013, 08:32
 */

#ifndef CCP1_ENGINE_RPM_H
#define	CCP1_ENGINE_RPM_H

#ifdef	__cplusplus
extern "C" {
#endif

extern inline void CCP1_vIsr(void);
extern inline void CCP1_vInit(void);

extern volatile unsigned int uiPrevT1Val;
extern volatile unsigned int uiCurrT1Val;

extern volatile unsigned char   ucSpeed;

#define ABS_DIFF(a,b) ((a) > (b) ? ((a) - (b)) : ((b) - (a))
#define GET_ENGINE_RPM  ( (uiPrevT1Val) > (uiCurrT1Val) ? ((uiPrevT1Val) - (uiCurrT1Val)) : ((uiCurrT1Val) - (uiPrevT1Val)) )

#ifdef	__cplusplus
}
#endif

#endif	/* CCP1_ENGINE_RPM_H */

