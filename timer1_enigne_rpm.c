#include <xc.h>
#include "config.h"
#include "led_seg.h"
#include "timer1_engine_rpm.h"

/**
 * In capture mode value of 16bit Timer1 is latched into CCPR1H and CCPR1L registers at every rising edge of CCP1 port
 *
 * Engine RPM circuit generates 2 short positive pulses per rotation.
 *             __                     __                     __
 * ...________|  |___________________|  |___________________|  |__...
 *
 *            |<-------------------->| measured time
 *            |<------------------------------------------->|  one full rotation time
 *
 * Lets assume that engine can reach 10k RPM. Working range can be from 1k to 10k RPM.
 * At 1k  RPM =  16 R per second. One turn takes 1/16s   = 62,5 ms
 * At 10k RPM = 166 R per second. One turn takes 1/166s  =  6   ms
 *
 * With 2 pulses per rotation, we need to measure time from 31 to 3 ms
 *
 * 16 bit TIMER1 without prescaler (1:1) gives overflow period about 65 ms.
 * With this timer we can measure from almost 500 RPM
 * 
 */

//TODO check for timer1 OV without CCP1 - means engine stuck

volatile unsigned int uiPrevT1Val;
volatile unsigned int uiCurrT1Val;





/**
 * Routine used to calulate engine RPM.
 * On every rising edge, TIMER1 value is stored in @ref uiCurrT1Val,
 * old value is copied to @ref uiPrevT1Val
 */
inline void CCP1_vIsr(void)
{
   if (PIR1bits.CCP1IF)
   {
//       TMR1H = 0;
//       TMR1L = 0;
//       uiPrevT1Val  = uiCurrT1Val;
//       uiCurrT1Val  = CCPR1H << 8 | CCPR1L;
//
//       if (stDisp.bDec2)
//       {
//           stDisp.bDec2 = 0;
//       }
//       else
//       {
//           stDisp.bDec2 = 1;
//       }
//       PIR1bits.CCP1IF = 0;
   }

}

/**
 * CCP2 works in output compare mode (generating pulses)
 */
inline void CCP2_vIsr(void)
{
   if (PIR2bits.CCP2IF)
   {
       if ( (CCP2CON & 0b00001111) == 0b00001000 ) // if previous mode was "compare, set output on match"
       {
            // set mode "compare, clear output on match"
            // calculate switch off time as value relative to current TIMER1 value (16 bit arithmetic with wrap around)
            CCP2_vSetWhenT1 (0, (unsigned int)T1_uiGet() + (unsigned int)TRIAC_GATE_PULSE_LEN_T1_CYCLES); // stop generating pulse after
       }
       else
       {
            CCP2_vInitAndDisable();
       }
       PIR2bits.CCP2IF = 0;
   }
}

inline void T1_vIsr(void)
{
//   if (PIR1bits.TMR1IF)
//   {
//       PIR1bits.TMR1IF = 0;
//   }
}

/**
 * set timer1 registers to given value
 * NOTE: timer1 is stopped during set
 * @param uiVal - 16 bit value to be written into TMR1H and TMR1L
 */
inline void T1_vSet(unsigned int uiVal)
{
    T1CONbits.TMR1ON = 0;
    TMR1H = uiVal >> 8;
    TMR1L = uiVal & 0xFF;
    T1CONbits.TMR1ON = 1;
}

/**
 * Read Timer1 value and return 16bit value
 *
 * Timer 1 overflow can occur between reading two 8 bit registers
 */
inline unsigned int T1_uiGet(void)
{
#if 1
    // Microchip proposed method of safe reading of running Timer1
    unsigned char ucHI, ucLO;
    ucHI = TMR1H;
    ucLO = TMR1L;
    if (TMR1H != ucHI)
    {
        // overflow occurs. reread values
        ucHI = TMR1H;
        ucLO = TMR1L;
    }
    return ucLO | (ucHI << 8);
    
#else
    unsigned int uiT1;

    // reding in steps to ensure reading order
    uiT1  = TMR1L;
    uiT1 |= TMR1H << 8;

    return uiT1;
#endif
}

/**
 * 
 */
inline void T1_vInit(void)
{
    //TIMER 1 initialisation (generated from: http://eng-serve.com/pic/pic_timer.html)
    //Timer1 Registers Prescaler= 1 - TMR1 Preset = 0 - Freq = 15.26 Hz - Period = 0.065536 seconds
    T1CONbits.T1CKPS1 = 0;   // bits 5-4  Prescaler Rate Select bits
    T1CONbits.T1CKPS0 = 0;   // bit 4
    T1CONbits.T1OSCEN = 0;   // bit 3 Timer1 Oscillator Enable Control bit 0 = off
    T1CONbits.T1SYNC = 1;    // bit 2 Timer1 External Clock Input Synchronization Control bit...1 = Do not synchronize external clock input
    T1CONbits.TMR1CS = 0;    // bit 1 Timer1 Clock Source Select bit...0 = Internal clock (FOSC/4)
    T1CONbits.TMR1ON = 1;    // bit 0 enables timer
    TMR1H = 0;               // preset for timer1 MSB register
    TMR1L = 0;               // preset for timer1 LSB register
    //PIE1bits.TMR1IE = 1;     // enable Timer1 overflow int
    PIR1bits.TMR1IF = 0;     // clear Timer1 INT flag

    INTCONbits.PEIE = 1; // enable peripheral interrupts (to get TIMER1 overflow int)
}

/**
 * Init CCP1 as Capture Mode (mesaure time)
 */
inline void CCP1_vInit(void)
{
    TRISCbits.TRISC2 = 1; // as input

    // CCP1 Initialisation
    // configure Capture mode on every rising edge odf CCP1 pin, no prescaler
    CCP1CONbits.CCP1M2 = 1;
    CCP1CONbits.CCP1M0 = 1;

    PIR1bits.CCP1IF = 0; // clear interrupt flag
    PIE1bits.CCP1IE = 1; // enable interrupt on CCP1 event

}

/**
 * Disable CCP2 module
 */
inline void CCP2_vInitAndDisable(void)
{
    // disable interrupt on CCP2 (false int can be generated when mode is changed)
    PIE2bits.CCP2IE = 0;
    CCP2CONbits.CCP2M3 = 0;
    CCP2CONbits.CCP2M2 = 0;
    CCP2CONbits.CCP2M1 = 0;
    CCP2CONbits.CCP2M0 = 0;
    TRIAC_OFF
}


/**
 * Program CCP2 to set CCP2 output pin to bState when T1 equals uiExpectedT1Value

 * @param bState
 * @param uiExpectedT1Value
 */
inline void CCP2_vSetWhenT1(unsigned char bState, unsigned int uiExpectedT1Value)
{
    // disable interrupt on CCP2 (false int gan be genrated when mode is changed)
    PIE2bits.CCP2IE = 0; 

    CCPR2H = uiExpectedT1Value >> 8;
    CCPR2L = uiExpectedT1Value & 0xFF;
    
    // configure mode:
    //   0b1000 - set output on match
    //   0b1001 - clear output on match
    CCP2CONbits.CCP2M3 = 1;
    CCP2CONbits.CCP2M2 = 0;
    CCP2CONbits.CCP2M1 = 0;
    if (bState==0)
    {
        CCP2CONbits.CCP2M0 = 1;
    }
    else
    {
        CCP2CONbits.CCP2M0 = 0;
    }

    //PIR2bits.CCP2IF = 0; // clear interrupt flag
    PIE2bits.CCP2IE = 1; // enable interrupt on CCP1 event

}