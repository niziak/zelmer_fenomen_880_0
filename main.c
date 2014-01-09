/*
 * File:   main.c
 * Author: nizinski_w
 *
 * Created on 5 grudzień 2013, 09:55
 */


#include <xc.h>
#include "conf_bits.h"

#include "config.h"
#include "led_seg.h"
#include "delay.h"
#include "timer0_led_key.h"
#include "timer1_engine_rpm.h"
#include "ext_int.h"
#include "key.h"
#include "equipment.h"



volatile unsigned char   bEngineOn;                 ///< enable engine power and disable engine brake
volatile unsigned char   bTriacOn;                  ///< permits EINT and CCP2 module generate gating pulses
volatile unsigned char   bOverheat;                 ///< engine overheat
volatile unsigned char   bWrongEq;                  ///< wrong equipment (sensors broken?)
volatile unsigned char   bGoodEq;                   ///< equipment are valid and close (possible to turn on engine)
volatile unsigned char   ucSelectedEngineSpeed;     ///< variable to keep user selected speed 0..12
//volatile unsigned char   ucCurrentEngineSpeed;      ///< variable to keep user selected speed 0..12
volatile unsigned int    uiCurrentTriacDelayInT1;   ///< current triac pulse delay from ZC in ticks of T1 timer
volatile unsigned int    uiDesiredTriacDelayInT1;   ///< desired (by user) triac pulse delay from ZC in ticks of T1 timer

volatile unsigned int    uiOnePulseCountdown;   ///<
volatile unsigned char   bMainTrigger;          ///< T0 trigger for main loop (active when 0)
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

void vChangeSpeedStep(void)
{
    // spinning down - increasing delay - reducing power
    if (uiCurrentTriacDelayInT1 < uiDesiredTriacDelayInT1)
    {
        uiCurrentTriacDelayInT1++;
    }
    //spinning up - decreasing delay - increasing power
    if (uiCurrentTriacDelayInT1 > uiDesiredTriacDelayInT1)
    {
        uiCurrentTriacDelayInT1--;
    }
}


/**
 * Engine speed defined as delay in Timer1 ticks after zero-crossing
 *
 */

const unsigned int auiSpeedTable[12] =
{
    DELAY_ZC_01 ,
    DELAY_ZC_02 ,
    DELAY_ZC_03 ,
    DELAY_ZC_04 ,
    DELAY_ZC_05 ,
    DELAY_ZC_06 ,
    DELAY_ZC_07 ,
    DELAY_ZC_08 ,
    DELAY_ZC_09 ,
    DELAY_ZC_10 ,
    DELAY_ZC_11 ,
    DELAY_ZC_12 ,
};


/**
 * Recalulate system state:
 *  - soft start / soft stop
 *  - handle pulse / autopulse counters
 */
void vUpdateSystemState(void)
{
        uiDesiredTriacDelayInT1 = auiSpeedTable[ucSelectedEngineSpeed-1];
        /////////////////////////////////////////
        // soft start
        if (ucSpeedStepChangeTimer > 0)
        {
                ucSpeedStepChangeTimer--;
                if (ucSpeedStepChangeTimer == 0)
                {
                    vChangeSpeedStep();
                }
        }
        
        if (uiCurrentTriacDelayInT1 != uiDesiredTriacDelayInT1)
        {
            if (ucSpeedStepChangeTimer == 0)
            {
                ucSpeedStepChangeTimer = SPEED_CHANGE_STEP_INTERVAL_IN_T0_OV_CYCLES;
            }
        }

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
            stDisp.bDec2 = 1;
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

            stDisp.bDec2 = 0;
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

    LED_Disp (SEG_NONE, SEG_H); delay_ms(500);
    LED_Disp (SEG_H,    SEG_E); delay_ms(500);
    LED_Disp (SEG_E,    SEG_L); delay_ms(500);
    LED_Disp (SEG_L,    SEG_0); delay_ms(500);
    LED_Disp (SEG_0, SEG_NONE); delay_ms(500);


    while (1)
    {
        // wait for bMainTrigger to be ZERO
        while (1 == bMainTrigger);
            bMainTrigger = 1;
            
            vScanKeyAndHandle();
            vUpdateSystemState();
            vUpdateEquipmentState();
            vStartStopEngine();
            vUpdateDisplay();

            //vDebugEquipmentState();

            LED_DispHex ( ((unsigned int)(uiPrevT1Val - uiCurrT1Val)) >> 8); // show MSB
    }
    return;
}
