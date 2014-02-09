#include <xc.h>
#include "config.h"
#include "globals.h"
#include "led_seg.h"

#define CHECK_H1    (0 == PORTAbits.RA2)
#define CHECK_H2    (0 == PORTAbits.RA3)
#define CHECK_H3    (0 == PORTAbits.RA1)
#define CHECK_HC    (0 == PORTAbits.RA0)

#define ENABLE_H3   {PORTCbits.RC6 = 1;}
#define DISABLE_H3  {PORTCbits.RC6 = 0;}


void vDebugEquipmentState(void)
{
    acDispContent[0] = (~PORTA & 0x0F);
}

void vUpdateEquipmentState(void)
{
    bWrongEq = 0;
    bGoodEq = 0;

    if (CHECK_H1 || CHECK_H2)
    {
        ENABLE_H3
        {
            bGoodEq = 1;
        }
    }
    else
    {
        DISABLE_H3
        if (CHECK_H3) // not possible to get H3 without enabling it
        {
            bWrongEq = 1;
        }
    }


}