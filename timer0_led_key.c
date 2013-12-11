#include <xc.h>
#include "timer1_engine_rpm.h"

/**
 * With external crystal:
 * Fout = Fclk / (Prescaler * (256-TMR0) * Count)
 *
 * With 4MHz crystal, prescaler 1:16, TMR0=0 should give 244,14Hz
 * http://eng-serve.com/pic/pic_timer.html
 *
 */

/**
 * Keyboard matrix:
 *
 *  Left display digit RC4 (active low)
 *   |
 *   |      Right display digit RC5 (Active low)
 *   |        |
 *   |        |                2 color LED common RC0 (active low)
 *   |        |                |
 *  [-]------[+]-------------[Turbo]--------- RA4 (pull up to 5V, so low when key pressed)
 *   |        |                |
 *   |        |                |
 *  [PULSE]--[AUTO]----------[ON/OFF]-------- RC3 (pull up to 5V, so low when key pressed)
 *
 * Keyboard is scanned by selecting column (low state) from left to right.
 * During scan, state of RA4 and RC3 is checked, to detect key press.
 * 
 *
 * One side of contact from all keys are connected to PLUS through resistors.
 * Second contacts from [-] and [+] keys are connected through ->>|- diode to RC4 and RC5
 *
 * Bits order in @ref eKeyboardState:
 *                 6      5        4         3         2         1
 *               [ON]  [TURBO]   [AUTO]     [+]     [PULSE]     [-]
 *
 * Key is pressed, when corresponding bit is set to 0
 */

#include "led_seg.h"

typedef enum
{
    T0_MODE_1ST_DIGIT = 0,      ///< first digit and key column [-] and [PULSE]
    T0_MODE_2ND_DIGIT = 1,      ///< second digit and key column [+] and [AUTO]
    T0_MODE_LED       = 2,      ///< bicolor LED and key column [TURBO] and [ON/OFF]
} T0_MODE_DEF;

typedef enum
{
    KEY_NONE    = 0x3F,         ///< 0011 1111  - no key pressed, all key bits are 1
            
    KEY_MINUS   = 0x3E,         ///< 0011 1110
    KEY_PULSE   = 0x3D,         ///< 0011 1101
    KEY_PLUS    = 0x3B,         ///< 0011 1011
    KEY_AUTO    = 0x37,         ///< 0011 0111
    KEY_TURBO   = 0x2F,         ///< 0010 1111
    KEY_START   = 0x1F,         ///< 0001 1111

    KEY_LAST    = 0xFF,         // to force at least 8 bit enum
} KEY_DEFS;

static volatile T0_MODE_DEF eMode;          ///< what to do in current ISR call

#define KEY_DEBOUNCE_WAIT_MS    20          ///< time in ms, to confirm that key is still pressed
#define KEY_DEBOUNCE_WAIT       ((KEY_DEBOUNCE_WAIT_MS)/12)   ///< ISR works at 244Hz = every 4 ms x 3 columns =  12 ms.

// TODO with every turn, store 2 bit of key state into 2 next bits - 6 bits only are needed
inline static KEY_DEFS eGetKey(void)
{
    static volatile unsigned char ucKeyPressTime; ///< debounce counter
    static volatile KEY_DEFS eKeyboardState;      ///< variable to collect key states during column scan see: @ref KEY_DEFS
    static volatile KEY_DEFS ekeyboardStateOld;   ///<

    // first column, clear keyboard state
    if (T0_MODE_1ST_DIGIT == eMode)
    {
        eKeyboardState = 0;
    }
    // for every scanned column, put 2 bits with rows status into one byte 
    eKeyboardState |= ((PORTAbits.RA4) | (PORTCbits.RC3) << 1) << (2*eMode);

    // after reading last column, we have complete keyboard picture in @ref ekeyboardState
    if (T0_MODE_LED == eMode)
    {
//        LED_DispHex(eKeyboardState);
        // if key is pressed
        if (KEY_NONE != eKeyboardState)
        {
            // remember keyboard state in temporary value
            if (ucKeyPressTime==0)
            {
                ekeyboardStateOld = eKeyboardState;
            }

            // count debounce time, but prevent from counter oveflow
            if (ucKeyPressTime<=KEY_DEBOUNCE_WAIT)
            {
                ucKeyPressTime++;
            }
            else
            // after debounce time, compare if the same key is still pressed
            {
                if (ekeyboardStateOld != eKeyboardState)
                {
                    ekeyboardStateOld = KEY_NONE;
                }
            }
        }
        else
        {
            // key is not pressed or released
            ucKeyPressTime = 0;
            eKeyboardState = ekeyboardStateOld; // use eKeyboardState as  temp value, to clear and return ekeyboardStateOld
            ekeyboardStateOld = KEY_NONE;
            return eKeyboardState;
        }
    }
    return KEY_NONE;
}



inline void T0_vIsr(void)
{
    if (INTCONbits.T0IF)
    {
        // read keys before changing eMode
        switch (eGetKey())
        {
            default:
                break;

            case KEY_MINUS:
                ucSpeed--;
                break;
                
            case KEY_PLUS:
                ucSpeed++;
                break;
        }

//        if (PORTAbits.RA4==0)
//        {
//            acDispContent[0] = SEG_A;
//            acDispContent[1] = SEG_4;
//        }
//        if (PORTCbits.RC3==0)
//        {
//            acDispContent[0] = SEG_C;
//            acDispContent[1] = SEG_3;
//        }

        eMode++;
        if (eMode > T0_MODE_LED)
        {
            eMode = T0_MODE_1ST_DIGIT;
        }

        DSP_DIS_DEC // turn off dot
        switch (eMode)
        {
            case T0_MODE_1ST_DIGIT:
                // we come here from TO_MODE_LED, so disable LED before changing state
                DSP_DISABLE
                PORTB = acDigitToSegMap[acDispContent[0]];
                if (stDisp.bDec1)
                {
                    DSP_EN_DEC
                }
                DSP_EN_1ST
                break;

            case T0_MODE_2ND_DIGIT:
                PORTB = acDigitToSegMap[acDispContent[1]];
                if (stDisp.bDec2)
                {
                    DSP_EN_DEC
                }
                DSP_EN_2ND
                break;

            case T0_MODE_LED:
                // to prevent ghost characters, disable all segments, then enable bicolor LED.
                PORTB = 0b11111110;
                DSP_DISABLE
                PORTB = LED_NONE ;
                if (stDisp.bGreen)
                {
                    PORTB |= LED_GREEN;
                }
                if (stDisp.bRed)
                {
                    PORTB |= LED_RED;
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
