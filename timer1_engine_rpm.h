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

extern inline void CCP1_vInit(void);
extern inline void CCP1_vIsr(void);

extern inline void T1_vInit(void);
extern inline void T1_vIsr(void);
extern inline void T1_vSet(unsigned int uiVal);
extern inline unsigned int T1_uiGet(void);

extern inline void CCP2_vInitAndDisable(void);
extern inline void CCP2_vIsr(void);
extern inline void CCP2_vSetWhenT1(unsigned char bState, unsigned int uiExpectedT1Value);




#define GET_ENGINE_RPM  ( (uiPrevT1Val) > (uiCurrT1Val) ? ((uiPrevT1Val) - (uiCurrT1Val)) : ((uiCurrT1Val) - (uiPrevT1Val)) )

#ifdef	__cplusplus
}
#endif

#endif	/* CCP1_ENGINE_RPM_H */

