/*
 * File:   main.c
 * Author: nizinski_w
 *
 * Created on 5 grudzień 2013, 09:55
 */


#include <xc.h>
#include "conf_bits.h"

#include "config.h"
#include "globals.h"
#include "led_seg.h"
#include "delay.h"
#include "timer0_led_key.h"
#include "timer1_engine_rpm.h"
#include "ext_int.h"
#include "key.h"
#include "equipment.h"




static unsigned char     ucSpeedStepChangeTimer;

/**
 * Durint interrupt handler GIE is automatically disabled
 */
void interrupt myIsr(void)
{
    CCP2_vIsr();
    EINT_vIsr();

    CCP1_vIsr();
    T1_vIsr();
    
    T0_vIsr();
}

/**
 * Engine speed defined as expected opto sensor pulse interval measured in T1 ticks
 *
 */
const unsigned int auiTachoLockValuesForSpeed[12] =
{
    0xE000,
    0xE200,
    0xE400,
    0xE600,
    0xE800,
    0xEA00,
    0xEC00,
    0xEE00,
    0xF100,
    0xF300,
    0xF500,
    0xF700,
};


#define INCREASE_SPEED(x)  {uiCurrentTriacDelayInT1 -= x;}
#define DECREASE_SPEED(x)  {uiCurrentTriacDelayInT1 += x;}

void vStabiliseSpeed(void)
{
    int iError;
    static int iLastError;
    static int iIntegral;       ///< sum of error
    int iDerivative;
    int iCorrection;

    #if CONFIG_SHOW_RPM_STABIL_ON_DOTS
        stDisp.bDec2 = 0;
        stDisp.bDec1 = 0;
    #endif
    

    // change one speed up/down means change (in uiError) by 0x200 = 512

    // PID calculations

    // Proportional part //
    // iError/10 - hard speed up/down - stable oscillation: 1/4 sec
    // iError/32 - soft unfinished oscilaltion: 1/2 sec
    // iError/50 - very soft finished oscillations - stability reached after 7 seconds
    #define CRITICAL_GAIN   1   ///< it is always 1 because @ref SCALE is calibrated to make oscillation
    #define SCALE 32            ///< some empirical value which makes relation between tacho error and @ref uiCurrentTriacDelayInT1
    
    #define ONE_IS (100) /// to remove floating point - all values are represented in 1/100

   
    #define OSC_PERIOD  (1)    ///< dt = 1/2 sec = 
    #define DT          (1)    ///< sample interval - for simplicity it is one now

    #define P_FACTOR ((100)     * CRITICAL_GAIN)
    #define I_FACTOR ((10)     * OSC_PERIOD)*0
    #define D_FACTOR ((12)     * OSC_PERIOD)*0

    //////////////////////////////////////////////////////////////////////
    // Proportional term - make corrections proportional to error and error sign
    iError = uiTachoLockValue - uiTachoCurrentValue;
    //iError = iError / SCALE;


    //////////////////////////////////////////////////////////////////////
    // Integral term - sum of errors - so make error correction faster (overshoot possible) or remove error offset
    //                 by accumulating small errors to bigger value. SHould be small to prevent overshoot but still possible
    //                 to correct small errors in reasonable time
    iIntegral += iError * DT; // TODO clear iIntegral when error is zero or zero crossing (to prevent overshoot)
    if (iError == 0)
    {
        iIntegral = 0;
    }
    // if error changes sign
    if (    ( (iError>0) && (iLastError<0) )
         || ( (iError<0) && (iLastError>0) ) )
    {
        iIntegral = 0;
    }


    ///////////////////////////////////////////////////////////////////////
    // Derivative term - slow down after a while to prevent overshoot
    iDerivative = (iError - iLastError) / DT;
    


    iLastError = iError;

    iCorrection  = iError      * P_FACTOR;
    iCorrection += iIntegral   * I_FACTOR;
    iCorrection += iDerivative * D_FACTOR;
    
    iCorrection /= ONE_IS; // rescale

//TEMP:
    iCorrection = iError;  // working reference - oscillating but works OK! iCorrection = iError / 32;

    //if ((-1 > iError) || (iError > 1))
    {
        uiCurrentTriacDelayInT1 -= iCorrection / SCALE;
    }

    #if CONFIG_SHOW_RPM_STABIL_ON_DOTS
        // if RPM is lower than required
        if (uiTachoCurrentValue > uiTachoLockValue)
        {
            stDisp.bDec1 = 1;
        }
        else
        if (uiTachoCurrentValue < uiTachoLockValue)
        {
            stDisp.bDec2 = 1;
        }
    #endif

    // check minimum and maximum allowed values
    if (uiCurrentTriacDelayInT1 > DELAY_ZC_LOWEST_POWER)
    {
        uiCurrentTriacDelayInT1 = DELAY_ZC_LOWEST_POWER;
    }
    if (uiCurrentTriacDelayInT1 < DELAY_ZC_HIGHEST_POWER)
    {
        uiCurrentTriacDelayInT1 = DELAY_ZC_HIGHEST_POWER;
    }
}




/**
 * Recalulate system state:
 *  - soft start / soft stop
 *  - handle pulse / autopulse counters
 */
void vUpdateSystemState(void)
{
        uiTachoLockValue = auiTachoLockValuesForSpeed[ucSelectedEngineSpeed-1];
        
        /////////////////////////////////////////
        // soft start
//        if (ucSpeedStepChangeTimer > 0)
//        {
//                ucSpeedStepChangeTimer--;
//                if (ucSpeedStepChangeTimer == 0)
//                {
//                    vChangeSpeedStep();
//                }
//        }
//
//        if (uiCurrentTriacDelayInT1 != uiDesiredTriacDelayInT1)
//        {
//            if (ucSpeedStepChangeTimer == 0)
//            {
//                ucSpeedStepChangeTimer = SPEED_CHANGE_STEP_INTERVAL_IN_T0_OV_CYCLES;
//            }
//        }

        /////////////////////////////////////////
        // decrement autopulse counter
        if (uiOnePulseCountdown>0)
        {
            uiOnePulseCountdown--;
            if (uiOnePulseCountdown==0)
            {
                bEngineOn = 0;
            }
        }



}

void vUpdateDisplay(void)
{
       ////////////////////////
        // present system state to user
        stDisp.bLedBlink = 0;
        stDisp.bLedGreen = 0;
        stDisp.bLedRed = 0;
        if (bGoodEq)
        {
            stDisp.bLedGreen = 1;
            LED_DispHex (BIN2BCD(ucSelectedEngineSpeed));
        }
        else
        {
            stDisp.bLedRed = 1;
            LED_Disp (SEG_NONE, SEG_0);
        }

        if (bWrongEq)
        {
            stDisp.bLedRed = 1;
            LED_Disp (SEG_E, SEG_R);
        }

        if (bOverheat)
        {
            stDisp.bLedGreen = 0;
            stDisp.bLedRed = 1;
            stDisp.bLedBlink = 1;
            LED_Disp (SEG_H, SEG_NONE);
        }
}

/**
 * Check system state and permit engine on, otherwise stop engine
 * or emergency stop engine (with brake)
 *
 * Engine is stopped by stoping generation of gating pulse to triac,
 * without touching engine relay
 *
 * Overheat is also checked here
 */
void vStartStopEngine(void)
{
        // check overheat condition
#if (CONFIG_CHECK_OVERHEAT)
        if (ucEINTGap > EINT_EXPECTED_T0_OV_CYCLES)
        {
            bOverheat = 1;
        }
        else
#endif
        {
            bOverheat = 0;
            ucEINTGap++;
        }


        if (    ( 0 == bOverheat )
             && ( 1 == bGoodEq   )
             && ( 0 == bWrongEq  )
             && ( 1 == bEngineOn ) )
        {
#if CONFIG_SHOW_ON_OFF_ON_DOT
            stDisp.bDec2 = 1;
#endif
            ENGINE_RELAY_ON
            bTriacOn = 1;
        }
        else
        {
            // check emergency braking condition:
            if (    ( 1 == bWrongEq )
                 || ( 0 == bGoodEq  ) )
            {
                ENGINE_RELAY_OFF //TODO disable relay also when engine is gently stopped
                ucSelectedEngineSpeed = 1; // after emergency break, start from lowest speed //TODO min and max speed limits from equipment
            }
#if CONFIG_SHOW_ON_OFF_ON_DOT
            stDisp.bDec2 = 0;
#endif
            bEngineOn = 0;  // delete user choice to turn engine on
            bTriacOn = 0;
            CCP2_vInitAndDisable(); // will call TRIAC_OFF macro
            uiCurrentTriacDelayInT1 = DELAY_ZC_01; // always start from lowest speed
        }
}

void main(void) {
    bGoodEq = 0;
    bWrongEq = 1;
    ucSelectedEngineSpeed = MIN_ENGINE_SPEED;
    
    TRISA = TRISB = TRISC = 0xFF; // as inputs (safe to start)
    PORTA = PORTB = PORTC = 0; // low state

    ANSEL = ANSELH = 0; // configure analog as digital


    //        76543210
    TRISA = 0b00001111; //RA3-RA0 as inputs - hall sensors from equipment
    TRISB = 0b00000001; //RB7-RB1 as outpus; RB0 as input (EXT INT)

    TRISCbits.TRISC4 = 0; // common anode first digit
    TRISCbits.TRISC5 = 0; // common anode second digit
    TRISCbits.TRISC0 = 0; // common anode bicolor LED
    TRISCbits.TRISC7 = 0; // RC7 as output - decimal point

    TRISCbits.TRISC3 = 1; // C3 as input (key)
    TRISAbits.TRISA4 = 1; // A4 as input (key)

    TRISAbits.TRISA5 = 0; // A5 as output - engine relay
    TRISCbits.TRISC1 = 0; // C5 as output - triac gate

    TRISCbits.TRISC6 = 0; // C6 as output - power for hall sensors
    PORTCbits.RC6 = 0;    // disable H3 and HC power

    EINT_vInit();
    T0_vInit();

    CCP1_vInit();
    CCP2_vInitAndDisable();
    T1_vInit();
    
    ei();   // global interrupt enable

    LED_Disp (SEG_NONE, SEG_H); delay_ms(HELLO_ANIMATION_DELAY_MS);
    LED_Disp (SEG_H,    SEG_E); delay_ms(HELLO_ANIMATION_DELAY_MS);
    LED_Disp (SEG_E,    SEG_L); delay_ms(HELLO_ANIMATION_DELAY_MS);
    LED_Disp (SEG_L,    SEG_0); delay_ms(HELLO_ANIMATION_DELAY_MS);
    LED_Disp (SEG_0, SEG_NONE); delay_ms(HELLO_ANIMATION_DELAY_MS);
    LED_Disp (SEG_NONE, SEG_NONE); delay_ms(HELLO_ANIMATION_DELAY_MS);

    while (1)
    {
        // wait for signal from ICP ISR
        if (TRUE == bNewTachoValue)
        {
            vStabiliseSpeed();
            bNewTachoValue = FALSE;
        }
        
        // wait for signal from T0 ISR
        if (TRUE == bMainTrigger)
        {
            bMainTrigger = FALSE;

            

            vScanKeyAndHandle();
            vUpdateSystemState();
            vUpdateEquipmentState();
            vStartStopEngine();
            vUpdateDisplay();

            //vDebugEquipmentState();

            #if CONFIG_SHOW_TACHO_MSB
            LED_DispHex ( uiTachoCurrentValue >> 8); // show MSB
            #endif
            #if CONFIG_SHOW_TACHO_ERROR_MSB
            LED_DispHex ( (uiTachoLockValue - uiTachoCurrentValue) >> 8);
            #endif
        }
        
    }
    return;
}
