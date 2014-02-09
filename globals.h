/* 
 * File:   globals.h
 * Author: niziak
 *
 * Created on February 9, 2014, 9:18 PM
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#ifdef	__cplusplus
extern "C" {
#endif

extern volatile unsigned char   bOverheat;
extern volatile unsigned char   bEngineOn;
extern volatile unsigned char   bTriacOn;
extern volatile unsigned char   bOverheat;
extern volatile unsigned char   bWrongEq;
extern volatile unsigned char   bGoodEq;
extern volatile unsigned char   ucSelectedEngineSpeed;
//extern volatile unsigned char   ucCurrentEngineSpeed;
extern volatile unsigned int    uiCurrentTriacDelayInT1;   ///< current triac pulse delay from ZC in ticks of T1 timer
extern volatile unsigned int    uiTachoCurrentValue;
extern volatile unsigned int    uiTachoLockValue;          ///< locked (requested) tacho value


extern volatile unsigned int    uiOnePulseCountdown;       ///<
extern volatile unsigned char   bMainTrigger;

extern volatile unsigned char   bNewTachoValue;            ///< flag to signal new read from tacho


#ifdef	__cplusplus
}
#endif

#endif	/* GLOBALS_H */

