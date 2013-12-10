/*
 * File:   main.c
 * Author: nizinski_w
 *
 * Created on 5 grudzień 2013, 09:55
 */


#include <xc.h>

#include "conf_bits.h"
#include "led_seg.h"
#include "delay.h"
#include "timer0_led_key.h"
#include "ext_int.h"


/**
 *                                   MCLR 01 #####   ##### 28 RB7 ICSP DAT ---> 7 seg
 *            Hall Cover ----------> RA0  02 ############# 27 RB6 ICSP CLD ---> 7 seg & 2 color LED Anode
 *            Hall 3 --------------> RA1  03 ############# 26 RB5 ------------> 7 seg
 *            Hall 1 --------------> RA2  04 ############# 25 RB4 ------------> 7 seg
 *            Hall 2 --------------> RA3  05 ############# 24 RB3 ------------> 7 seg & 2 color LED Anode
 * Upper 3 keys (L when press) ----> RA4  06 ############# 23 RB2 ------------> 7 seg
 *           Relay <---------------- RA5  07 ############# 22 RB1 ------------> 7 seg
 *                               [-] Vss  08 ############# 21 RB0 INT <------- 0/5 V 50Hz cycle from mains (when absent - overheat)
 *                           XT ---- RA7  09 ############# 20 Vdd [+]
 *                           XT ---- RA6  10 ############# 19 Vss [-]
 *        2 color LED common K  <--- RC0  11 ############# 18 RC7 ------------> 7 seg dot
 *             Triac gate <--------- RC1  12 ############# 17 RC6
 *             Enigne RPM ----> CCP1 RC2  13 ############# 16 RC5 ------------> 7 seg right display plus to Anode + [-] Key
 * Lower 3 keys (L when press) ----> RC3  14 ############# 15 RC4 ------------> 7 seg left display plus to Anode + [+] Key
 *
 * All hall sensors in idle gives 5V, when magnet is detected voltage drops to zero.
 *
 * Enigne RPM are 2 positive pulses per rotation. So it is good to measure time between rising edges.
 */


void interrupt myIsr(void)
{
    EINT_vIsr();
    T0_vIsr();
}

void main(void) {
    TRISA = 0xFF; // as inputs
    TRISB = 0; 
    TRISC = 0xFF; // as inputs

    PORTA = PORTB = PORTC = 0; // low state

    ANSEL = ANSELH = 0; // configure analog as digital

    //                   76543210
    TRISBbits.TRISB0 = 0b00000001; //RB7-RB1 as outpus; RB0 as input (EXT INT)
    TRISCbits.TRISC4 = 0; // common anode first digit
    TRISCbits.TRISC5 = 0; // common anode second digit
    TRISCbits.TRISC0 = 0; // common anode bicolor LED
    TRISCbits.TRISC7 = 0; // RC7 as output - decimal point

    TRISCbits.TRISC3 = 1; //RC0 as input (key)
    TRISAbits.TRISA4 = 1; //RA4 as input (key)
    

    EINT_vInit();
    T0_vInit();
    ei();   // global interrupt enable

    LED_Disp (SEG_E, SEG_R);
    delay_ms(3000);
    
    while (1)
    {
        LED_Disp (SEG_NONE, SEG_NONE);
        delay_ms (1000);

        LED_Disp (SEG_P, SEG_A);
        delay_ms(500);
        LED_DispHex (PORTA);
        delay_ms(1000);

        LED_Disp (SEG_P, SEG_C);
        delay_ms(500);
        LED_DispHex (PORTC);
        delay_ms(1000);

        LED_Disp (SEG_C, SEG_NONE);
        delay_ms(500);
        LED_DispHex (ucCounter);
        delay_ms(1000);

    }
    
//    while (1)
//    {
//        if (ucZC) // if zero crossing detected
//        {
//            PORTCbits.RC0 = 1;
//            delay_ms(1);
//            PORTCbits.RC0 = 0;
//            ucZC = 0;
//        }
//    }
    return;
}
