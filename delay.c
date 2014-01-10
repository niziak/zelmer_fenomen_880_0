
#include <xc.h>
//#include <htc.h>
#include "config.h"
#ifndef _XTAL_FREQ
#error "Please define _XTAL_FREQ in config.h file"
#endif

void delay_ms(unsigned int uiDelayMs)
{
    while (uiDelayMs--)
    {
        __delay_ms(1);
    };
    //_delay((unsigned long)((uiDelayMs)*(_XTAL_FREQ/4000.0)));

    //__delay_ms(uiDelayMs);
}
