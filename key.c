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
#include <xc.h>
#include "config.h"
#include "key.h"
#include "timer0_led_key.h"

/**
 * Routine to be called from ISR.
 * For every selected column (by @ref eMode), 2 bit key row state is stored as 2 separate bits inside @ref eKeyboardState
 *
 * @return
 */
inline KEY_DEFS eGetKey(void)
{
    static volatile unsigned char ucKeyPressTime; ///< debounce counter
    static volatile KEY_DEFS eKeyboardState;      ///< variable to collect key states during column scan see: @ref KEY_DEFS
    static volatile KEY_DEFS ekeyboardStateOld;   ///<

    // first column, clear keyboard state
    if (COL_1ST_DIGIT == eSelectedColumn)
    {
        eKeyboardState = 0;
    }
    // for every scanned column, put 2 bits with rows status into one byte
    eKeyboardState |= ((PORTAbits.RA4) | (PORTCbits.RC3) << 1) << (2*eSelectedColumn);

    // after reading last column, we have complete keyboard picture in @ref ekeyboardState
    if (COL_LED == eSelectedColumn)
    {
//        LED_DispHex(eKeyboardState);
        // if key is pressed
        if (KEY_NONE != eKeyboardState)
        {
            // remember keyboard state in temporary value
            if (ucKeyPressTime == 0)
            {
                ekeyboardStateOld = eKeyboardState;
            }

            // count debounce time, but prevent from counter oveflow
            if (ucKeyPressTime <= KEY_DEBOUNCE_WAIT)
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

inline void vScanKeyAndHandle(void)
{
        // read keys before changing eMode
        switch (eGetKey())
        {
            default:
                break;

            case KEY_START:
                if (bEngineOn == 0)
                {
                    ucCurrentEngineSpeed = MIN_ENGINE_SPEED;
                    bEngineOn = 1;
                }
                else
                {
                    bEngineOn = 0;
                }
                break;

            case KEY_PULSE:
                // ignore key when previous auto pulse is not finished
                if (uiOnePulseCountdown==0)
                {
                    ucCurrentEngineSpeed = MIN_ENGINE_SPEED;
                    bEngineOn = 1;
                    uiOnePulseCountdown = MANUAL_PULSE_PERIOD_IN_T0_OV_CYCLES * (1+ucSelectedEngineSpeed / 12);
                }
                break;

            case KEY_AUTO:
                ucCurrentEngineSpeed = MIN_ENGINE_SPEED;
                break;

            case KEY_TURBO:
                ucCurrentEngineSpeed = MIN_ENGINE_SPEED;
                break;

            case KEY_MINUS:
                if (ucSelectedEngineSpeed > MIN_ENGINE_SPEED)
                    ucSelectedEngineSpeed--;
                break;

            case KEY_PLUS:
                if (ucSelectedEngineSpeed < MAX_ENGINE_SPEED)
                    ucSelectedEngineSpeed++;
                break;
        }
}