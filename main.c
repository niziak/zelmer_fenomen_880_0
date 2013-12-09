/*
 * File:   main.c
 * Author: nizinski_w
 *
 * Created on 5 grudzień 2013, 09:55
 */


#include <xc.h>
#include <htc.h>
#define _XTAL_FREQ 4000000

#include "conf_bits.h"
#include "led_seg.h"

/**
 * Keys:
 * 
 *  [-]      [+]               [Turbo]
 *  
 *
 *  [PULSE]  [AUTOPULSE]        [ON/OFF]
 *
 * One side of contact from all keys are connected to PLUS through resistors.
 * Second contacts from [-] and [+] keys are connected through ->|- diode to RC4 and RC5
 */

/**
 *                                   MCLR 01 #####   ##### 28 RB7 ICSP DAT ---- 7 seg
 *                                   RA0  02 ############# 27 RB6 ICSP CLD ---- 7 seg
 *                                   RA1  03 ############# 26 RB5 ------------- 7 seg
 *                                   RA2  04 ############# 25 RB4 ------------- 7 seg
 *                                   RA3  05 ############# 24 RB3 ------------- 7 seg
 *                                   RA4  06 ############# 23 RB2 ------------- 7 seg
 *                                   RA5  07 ############# 22 RB1 ------------- 7 seg
 *                                   Vss  08 ############# 21 RB0 INT -------- 0/5 V 50Hz cycle from mains
 *                                   RA7  09 ############# 20 Vdd
 *                                   RA6  10 ############# 19 Vss
 *                                   RC0  11 ############# 18 RC7 ------------- 7 seg
 *                                   RC1  12 ############# 17 RC6
 *                                   RC2  13 ############# 16 RC5 ------------- 7 seg right display plus to Anode + [-] Key
 *                                   RC3  14 ############# 15 RC4 ------------- 7 seg left display plus to Anode + [+] Key
 
 */



#define LED_RED     { }
#define LED_GREEN   { }

volatile unsigned char ucCounter;
volatile unsigned char ucZC;    ///< Zero Crossing flag

void interrupt myIsr(void)
{
    if (INTCONbits.INTF)    // external interrupt edge detected
    {
        ucZC = 1;
        ucCounter++;
        INTCONbits.INTF = 0;
    }
}

void main(void) {
    TRISA = TRISB = TRISC = 1; // as inputs
    PORTA = PORTB = PORTC = 0; // low state

    ANSEL = ANSELH = 0; // configure analog as digital

//    TRISBbits.TRISB0 = 1; // as input - external interrupt
//    PORTBbits.RB0 = 0;
//
//    OPTION_REGbits.INTEDG = 0; // EXT INT on falling edge
//    INTCONbits.INTF = 0; // clear EXT INT flag
//    INTCONbits.INTE = 1; // EXT INT enable
//    INTCONbits.GIE = 1; // global interrupt enable flag

    TRISB = 0b00000001; // RB as input
    unsigned char ucI;
    
    while (1)
    {
        DSP_EN_1ST
        DSP_EN_2ND
        PORTB = 0;
        __delay_ms(3000);

        for (ucI=0; ucI<12; ucI++)
        {
            DSP_EN_1ST
            __delay_ms(500);
            //_delay(100000);
            DSP_EN_2ND
            __delay_ms(500);
            //_delay(100000);
            PORTB = acDigitToSegMap[ucI];
        }
    }
    
    while (1)
    {
        if (ucZC) // if zero crossing detected
        {
            PORTCbits.RC0 = 1;
            __delay_ms(1);
            PORTCbits.RC0 = 0;
            ucZC = 0;
        }
    }
    return;
}
