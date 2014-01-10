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
#define _XTAL_FREQ 4000000      ///< in Hz

#define FOSC4_HZ      (_XTAL_FREQ/4)   ///< Crystal value after first system divisor /4
#define FOSC4_MICRO   (FOSC4_HZ/1000000)  ///< one Fosc4 tick is every X us

/**
 * 8bit Timer0 works Fosc /4 / divider
 *  4Mhz clock with 1:16 divider gives overflow every ~4ms
 */
#define T0_DIVIDER                         16
#define T0_OV_PERIOD_US                    (256 * T0_DIVIDER / FOSC4_MICRO)   // 4096 us
#define T0_OV_PERIOD_MS                    ((T0_OV_PERIOD_US)/1000)           // ~4ms
#if (T0_OV_PERIOD_MS<1)
#error "T0_OV_PERIOD_MS<1 !"
#endif

/**
 * 16bit Timer1 works at Fosc/4 / divider (prescaler)
 * Fosc = 4Mhz gives overflow every 65536 us with prescaler 1:1
 *                           
 */
#define T1_DIVIDER                         2  ///< timer1 prescaler (see T1 config in @ref T1_vInit)
#define T1_OV_PERIOD_US                    (float)(65536 * T1_DIVIDER / FOSC4_MICRO) // 65536 us  131072 us
//#define T1_OV_PERIOD_MS                  ((T1_OV_PERIOD_US)/1000)         // ~65ms with 1:1 ~ 128ms with 1:2

#define T1_CYCLES_FOR_US                   (float)(65536/T1_OV_PERIOD_US)   // gives 1/2 so use float
//#define T1_CYCLES_FOR_US                 1/2
//#define T1_CYCLES_FOR_MS                   (1000*(T1_CYCLES_FOR_US))

//#if (T1_CYCLES_FOR_MS<1)
//#error "T1_CYCLES_FOR_MS<1 !"
//#endif

/**
 * Engine speed defined as delay in Timer1 ticks after zero-crossing
 *
 * at 8111 engine is only buzzing, 7333 is ok for lowest possible but not usable
 */
#define DELAY_ZC(x)     (unsigned int)((x)*(T1_CYCLES_FOR_US))
//#define DELAY_ZC(x)     ((T1_CYCLES_FOR_US_X10)*(x)/10)

#define     DELAY_ZC_01     DELAY_ZC(7000)
#define     DELAY_ZC_02     DELAY_ZC(6611)
#define     DELAY_ZC_03     DELAY_ZC(6222)
#define     DELAY_ZC_04     DELAY_ZC(5889)
#define     DELAY_ZC_05     DELAY_ZC(5556)
#define     DELAY_ZC_06     DELAY_ZC(5278)
#define     DELAY_ZC_07     DELAY_ZC(4944)
#define     DELAY_ZC_08     DELAY_ZC(4611)
#define     DELAY_ZC_09     DELAY_ZC(4278)
#define     DELAY_ZC_10     DELAY_ZC(3944)
#define     DELAY_ZC_11     DELAY_ZC(3611)
#define     DELAY_ZC_12     DELAY_ZC(3278)





#define KEY_DEBOUNCE_WAIT_MS            10          ///< time in ms, to confirm that key is still pressed
#define KEY_DEBOUNCE_WAIT               ((KEY_DEBOUNCE_WAIT_MS)/(T0_OV_PERIOD_MS)*3)   ///< ISR works at 244Hz = every 4 ms x 3 columns =  12 ms.

#define EINT_EXPECTED_EVERY_MS          100         ///< if no EINT after this time - assume device overheat
#define EINT_EXPECTED_T0_OV_CYCLES      ((EINT_EXPECTED_EVERY_MS)/(T0_OV_PERIOD_MS))        ///< EINT expected to be received during XXX TIMER0 cycles

#define MANUAL_PULSE_PERIOD_MS             1000                                /// autopulse time in ms
#define MANUAL_PULSE_PERIOD_IN_T0_OV_CYCLES  ((MANUAL_PULSE_PERIOD_MS)/(T0_OV_PERIOD_MS))           ///< autopulse time in TIMER0 cycles

#define SPEED_CHANGE_STEP_INTERVAL_MS    5        ///< for soft start
#define SPEED_CHANGE_STEP_INTERVAL_IN_T0_OV_CYCLES    ((SPEED_CHANGE_STEP_INTERVAL_MS)/(T0_OV_PERIOD_MS))

#define TRIAC_GATE_PULSE_LEN_US          500
#define TRIAC_GATE_PULSE_LEN_T1_CYCLES   ((T1_CYCLES_FOR_US)*(TRIAC_GATE_PULSE_LEN_US))

#define MIN_ENGINE_SPEED                 1
#define MAX_ENGINE_SPEED                 12


#define TRIAC_OFF                       { PORTCbits.RC1 = 0; };

#define ENGINE_RELAY_ON                 { PORTAbits.RA5 = 1; };
#define ENGINE_RELAY_OFF                { PORTAbits.RA5 = 0; };

#define CONFIG_CHECK_OVERHEAT           1
#define CONFIG_SHOW_25HZ_ON_DOT         0   ///< change dot 1 state with every ext int
#define CONFIG_SHOW_RPM_PULSE_ON_DOT    1   ///< change dot 1 state with every CCP1 int
#define CONFIG_SHOW_T1_OV_ON_DOT        0   ///> change dot 1 state with every timer1 overflow

extern volatile unsigned char   bOverheat; 
extern volatile unsigned char   bEngineOn;
extern volatile unsigned char   bTriacOn;
extern volatile unsigned char   bOverheat;
extern volatile unsigned char   bWrongEq;
extern volatile unsigned char   bGoodEq;
extern volatile unsigned char   ucSelectedEngineSpeed;
//extern volatile unsigned char   ucCurrentEngineSpeed;
extern volatile unsigned int    uiCurrentTriacDelayInT1;   ///< current triac pulse delay from ZC in ticks of T1 timer
extern volatile unsigned int    uiDesiredTriacDelayInT1;   ///< desired (by user) triac pulse delay from ZC in ticks of T1 timer

extern volatile unsigned int    uiOnePulseCountdown;       ///<
extern volatile unsigned char   bMainTrigger;  

#define BIN2BCD(a)  ( (((a) / 10) << 4) + ((a) % 10))

#define ABS_DIFF(a,b) ((a) > (b) ? ((a) - (b)) : ((b) - (a))




#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

