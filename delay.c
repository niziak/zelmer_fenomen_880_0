
#include <xc.h>
#include <htc.h>
#define _XTAL_FREQ 4000000

void delay_ms(unsigned int uiDelayMs)
{
    while (uiDelayMs--)
    {
        __delay_ms(1);
    };
    //_delay((unsigned long)((uiDelayMs)*(_XTAL_FREQ/4000.0)));

    //__delay_ms(uiDelayMs);
}
