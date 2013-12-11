#include <xc.h>
#include "led_seg.h"

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

volatile unsigned char   ucSpeed;    ///< variable to keep user selected speed 0..12

/**
 * Routine used to calulate engine RPM.
 * On every rising edge, TIMER1 value is stored in @ref uiCurrT1Val,
 * old value is copied to @ref uiPrevT1Val
 */
inline void CCP1_vIsr(void)
{
   if (PIR1bits.CCP1IF)
   {
       TMR1H = 0;
       TMR1L = 0;
       uiPrevT1Val  = uiCurrT1Val;
       uiCurrT1Val  = CCPR1H << 8 | CCPR1L;

       if (stDisp.bDec2)
       {
           stDisp.bDec2 = 0;
       }
       else
       {
           stDisp.bDec2 = 1;
       }
       PIR1bits.CCP1IF = 0;
   }

   if (PIR1bits.TMR1IF)
   {
       PIR1bits.TMR1IF = 0;
   }
}

/**
 * Init CCP1 as Capture Mode
 */
inline void CCP1_vInit(void)
{
    TRISCbits.TRISC2 = 1; // as input

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
    PIE1bits.TMR1IE = 1; // enable Timer1 int
    PIR1bits.TMR1IF = 0;

    // CCP1 Initialisation
    // configure Capture mode on every rising edge odf CCP1 pin, no prescaler
    CCP1CONbits.CCP1M2 = 1;
    CCP1CONbits.CCP1M0 = 1;

    PIR1bits.CCP1IF = 0; // clear interrupt flah
    PIE1bits.CCP1IE = 1; // enable interrupt on CCP1 event

    INTCONbits.PEIE = 1; // enable peripheral interrupts (to get TIMER1 overflow int)

}