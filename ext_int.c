/**
 * Main sine wave:
 *     _________                       _________
 *    /         \                     /         \
 *   /           \                   /           \
 *  /             \                 /             \
 * ----------------+---------------+---------------+-------
 *                 .\             /.               .\
 *                 . \           / .               . \
 *                 .  \_________/  .               .
 *                 .               .               .
 * EXT INT leve:   .               .               .
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

volatile unsigned char ucCounter;
volatile unsigned char ucZC;    ///< Zero Crossing flag


inline void EINT_vIsr(void)
{
    if (INTCONbits.INTF)    // external interrupt edge detected
    {
        // Switch EXT INT to detect oposite edge
        if (OPTION_REGbits.INTEDG == 0)
        {
            OPTION_REGbits.INTEDG = 1; // EXT INT on raising edge
        }
        else
        {
            OPTION_REGbits.INTEDG = 0; // EXT INT on falling edge
        }
        ucZC = 1;
        ucCounter++;
        if (stDisp.bDec1)
        {
            stDisp.bDec1 = 0;
        }
        else
        {
            stDisp.bDec1 = 1;
        }
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