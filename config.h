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

#define T0_OV_PERIOD_US                    4096
#define T0_OV_PERIOD_MS                    ((T0_OV_PERIOD_US)/1000)         // ~4ms

#define T1_OV_PERIOD_US                    65536
#define T1_OV_PERIOD_MS                    ((T1_OV_PERIOD_US)/1000)         // ~65ms

#define T1_CYCLES_FOR_US                 ((T1_OV_PERIOD_US)/65535)
#define T1_CYCLES_FOR_MS                 ((T1_OV_PERIOD_US)*1000/65535)




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
#define CONFIG_SHOW_25HZ_ON_DOT         1


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


/**
 * Engine speed defined as delay in Timer1 ticks after zero-crossing
 *
 * at 8111 engine is only buzzing, 7333 is ok for lowest possible but not usable
 */
#define DELAY_ZC(x)     ((x)*T1_CYCLES_FOR_US)

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



#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

