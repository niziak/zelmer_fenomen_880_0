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
        stDisp.bGreen = ucCounter & 1;
        stDisp.bRed   = ucCounter & 2 > 1;
        INTCONbits.INTF = 0;
    }
}

void EINT_vInit(void)
{
    TRISBbits.TRISB0 = 1; // as input - external interrupt
    PORTBbits.RB0 = 0;

    OPTION_REGbits.INTEDG = 0; // EXT INT on falling edge
    INTCONbits.INTF = 0; // clear EXT INT flag
    INTCONbits.INTE = 1; // EXT INT enable
}