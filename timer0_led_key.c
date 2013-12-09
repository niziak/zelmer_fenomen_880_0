#include <xc.h>

/**
 * With external crystal:
 * Fout = Fclk / (Prescaler * (256-TMR0) * Count)
 *
 * With 4MHz crystal, prescaler 1:16, TMR0=0 should give 244,14Hz
 *
 */

/**
 * Keys:
 *
 *  Left display digit RC4
 *   |
 *   |      Right display digit RC5
 *   |        |
 *   |        |                2 color LED common RC0
 *   |        |                |
 *  [-]------[+]-------------[Turbo]--------- RA4 (pull up to 5V)
 *   |        |                |
 *   |        |                |
 *  [PULSE]--[AUTO]----------[ON/OFF]-------- RC3 (pull up to 5V)
 *
 * One side of contact from all keys are connected to PLUS through resistors.
 * Second contacts from [-] and [+] keys are connected through ->|- diode to RC4 and RC5
 */

#include "led_seg.h"
//                    76543210
#define LED_NONE    0b00000000
#define LED_RED     0b01000000      // RB6
#define LED_GREEN   0b00001000      // RB3
#define LED_ORANGE  0b01001000      // RB6+RB3

typedef enum
{
    T0_MODE_1ST_DIGIT,      ///< first digit and key column [-] and [PULSE]
    T0_MODE_2ND_DIGIT,      ///< second digit and key column [+] and [AUTO]
    T0_MODE_LED,            ///< bicolor LED and key column [TURBO] and [ON/OFF]
} T0_MODE_DEF;

static volatile T0_MODE_DEF eMode = 0;

void T0_vIsr(void)
{
    if (INTCONbits.T0IF)
    {
        if (PORTAbits.RA4==0)
        {
            acDispContent[0] = SEG_A;
            acDispContent[1] = SEG_4;
        }
        if (PORTCbits.RC3==0)
        {
            acDispContent[0] = SEG_C;
            acDispContent[1] = SEG_3;
        }
//        DSP_DISABLE
        PORTCbits.RC7 = 1; // turn off dot
        //        76543210
        switch (eMode)
        {
            case T0_MODE_1ST_DIGIT:
                // we come here from TO_MODE_LED, so disable LED before changing state
                DSP_DISABLE
                PORTB = acDigitToSegMap[acDispContent[0]];
                DSP_EN_1ST
                break;

            case T0_MODE_2ND_DIGIT:
                PORTB = acDigitToSegMap[acDispContent[1]];
                DSP_EN_2ND
                break;

            case T0_MODE_LED:
                // to prevent ghost characters, disable all segments, then enable bicolor LED.
                PORTB = 0b11111110;
                DSP_DISABLE
                PORTB = LED_GREEN;
                DSP_EN_LED
                break;
        }
        eMode++;
        if (eMode>T0_MODE_LED)
        {
            eMode = T0_MODE_1ST_DIGIT;
        }
        INTCONbits.T0IF = 0;
    }
}

void T0_vInit(void)
{
    OPTION_REGbits.PSA = 0; // prescaler assigned to T0 not to WDT
    OPTION_REGbits.PS0 = 1; //
    OPTION_REGbits.PS1 = 1; //
    OPTION_REGbits.PS2 = 0; // 011 - divide by 16
    OPTION_REGbits.T0SE = 0; // Edge selction: 0-rising
    OPTION_REGbits.T0CS = 0; // Clock selection: Internal instruction cycle clock
    TMR0=0;
    INTCONbits.T0IE = 1; // T0 interrupt enable
}
