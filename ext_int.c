/**
 * Mains sine wave:
 *     _________                       _________
 *    /         \                     /         \
 *   /           \                   /           \
 *  /             \                 /             \
 * ----------------+---------------+---------------+-------
 *                 .\             /.               .\
 *                 . \           / .               . \
 *                 .  \_________/  .               .
 *                 .               .               .
 * EXT INT level:  .               .               .
 *  _______________.               ._______________.
 * |               |_______________|               |________
 *
 *                 |<------------->| one half wave period = 10 ms
 *                 |       ########| expected Triac conduction time
 *                 |<----->|       | wait time before generating gate pulse
 *                 |       #       | triac gate pulse
 *
 * Workflow:
 *                 |<-             | EXT INT: set CCP2 to generate soft IRQ after XXX timer1 ticks
 *                 |       |<-     | CCP2 IRQ turn on triac gate,
 *                 |               | set CCP2 to generate soft IRQ after time needed to switch on triac (1ms?)
 *                 |        |<-    | CCP2 IRQ: switch off triac gate. Disable CCP2
 *
 */

#include <xc.h>
#include "ext_int.h"
#include "led_seg.h"
#include "timer0_led_key.h"
#include "timer1_engine_rpm.h"
#include "config.h"

/**
 * Engine speed defined as delay in Timer1 ticks after zero-crossing
 *
 * at 8111 engine is only buzzing, 7333 is ok for lowest possible but not usable
 */
#define DELAY_ZC(x)     ((x)*T1_CYCLES_FOR_US),


unsigned int auiSpeedTable[12] =
{
    DELAY_ZC(7000)
    DELAY_ZC(6611)
    DELAY_ZC(6222)
    DELAY_ZC(5889)
    DELAY_ZC(5556)
    DELAY_ZC(5278)
    DELAY_ZC(4944)
    DELAY_ZC(4611)
    DELAY_ZC(4278)
    DELAY_ZC(3944)
    DELAY_ZC(3611)
    DELAY_ZC(3278)
};

/**
 * External interrupt handler. EINT is generated when zero crossing occurs.
 */
inline void EINT_vIsr(void)
{
    if (INTCONbits.INTF)    // external interrupt edge detected
    {
        // measure time between EINTs
        ucEINTGap = 0;

        // Switch EXT INT to detect oposite edge
        if (OPTION_REGbits.INTEDG == 0)
        {
            OPTION_REGbits.INTEDG = 1; // EXT INT on raising edge
        }
        else
        {
            OPTION_REGbits.INTEDG = 0; // EXT INT on falling edge
        }

        CCP2_vInitAndDisable(); // switch off gate
        if (bTriacOn)
        {
            // calculate switch on time as value relative to current TIMER1 value (16 bit arithmetic with wrap around)
            CCP2_vSetWhenT1(1, (unsigned int)T1_uiGet() + (auiSpeedTable[ucCurrentEngineSpeed-1]) ); // generate triac pulse after delay time
        }
        else
        {
            //CCP2_vInitAndDisable();
        }

#if (CONFIG_SHOW_25HZ_ON_DOT)
        if (stDisp.bDec1)
        {
            stDisp.bDec1 = 0;
        }
        else
        {
            stDisp.bDec1 = 1;
        }
#endif //(CONFIG_SHOW_25HZ_ON_DOT)

        INTCONbits.INTF = 0;
    }
}

inline void EINT_vInit(void)
{
    TRISBbits.TRISB0 = 1; // as input - external interrupt
    PORTBbits.RB0 = 0;

    OPTION_REGbits.INTEDG = 0; // EXT INT on falling edge
    INTCONbits.INTF = 0; // clear EXT INT flag
    INTCONbits.INTE = 1; // EXT INT enable
}