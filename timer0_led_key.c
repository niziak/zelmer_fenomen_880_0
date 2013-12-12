/**
 * With external crystal:
 * Fout = Fclk / (Prescaler * (256-TMR0) * Count)
 *
 * With 4MHz crystal, prescaler 1:16, TMR0=0 should give 244,14Hz
 * http://eng-serve.com/pic/pic_timer.html
 *
 */



#include <xc.h>
#include "timer1_engine_rpm.h"
#include "config.h"
#include "led_seg.h"
#include "key.h"
#include "timer0_led_key.h"



volatile unsigned char ucEINTGap;           ///< counter to measure time between EINTs (incremented by T0 every 4ms)
volatile SCAN_COL_DEF eSelectedColumn;          ///< what to do in current ISR call






inline void T0_vIsr(void)
{
    static unsigned char ucBlinkState;

    if (INTCONbits.T0IF)
    {
        bMainTrigger = 0;

        ucBlinkState++;
        eSelectedColumn++;
        if (eSelectedColumn > COL_LED)
        {
            eSelectedColumn = COL_1ST_DIGIT;
        }

        DSP_DIS_DEC // turn off dot
        switch (eSelectedColumn)
        {
            case COL_1ST_DIGIT:
                // we come here from TO_MODE_LED, so disable LED before changing state
                DSP_DISABLE
                PORTB = acDigitToSegMap[acDispContent[0]];
                if (stDisp.bDec1)
                {
                    DSP_EN_DEC
                }
                DSP_EN_1ST
                break;

            case COL_2ND_DIGIT:
                PORTB = acDigitToSegMap[acDispContent[1]];
                if (stDisp.bDec2)
                {
                    DSP_EN_DEC
                }
                DSP_EN_2ND
                break;

            case COL_LED:
                // to prevent ghost characters, disable all segments, then enable bicolor LED.
                PORTB = 0b11111110;
                DSP_DISABLE
                PORTB = LED_NONE ;
                if (    (0 == stDisp.bLedBlink)
                     || (ucBlinkState & 0b1000000) )
                {
                    if (stDisp.bLedGreen)
                    {
                        PORTB |= LED_GREEN;
                    }
                    if (stDisp.bLedRed)
                    {
                        PORTB |= LED_RED;
                    }
                }
                DSP_EN_LED
                break;
        }
        INTCONbits.T0IF = 0;
    }
}

inline void T0_vInit(void)
{
    OPTION_REGbits.PSA = 0; // prescaler assigned to T0 not to WDT
    OPTION_REGbits.PS0 = 1; //
    OPTION_REGbits.PS1 = 1; //
    OPTION_REGbits.PS2 = 0; // 011 - divide by 16
    OPTION_REGbits.T0SE = 0; // Edge selction: 0-rising
    OPTION_REGbits.T0CS = 0; // Clock selection: Internal instruction cycle clock
    TMR0 = 0;
    INTCONbits.T0IF = 0; // clear interrupt flag
    INTCONbits.T0IE = 1; // T0 interrupt enable
}
