
#include <xc.h>
#include "led_seg.h"

// 2 block seven segment display is used
// also two color LED is connected to the same pins

const unsigned char acDigitToSegMap[] =
{
    // LED segment is turned ON with low state, so make negative bit values
    #define SEG_CHAR_DEF(enum,segments)     ~(segments),
        #include "led_seg_def.h"
    #undef SEG_CHAR_DEF
};



volatile SEG_CHAR_ID_DEF    acDispContent[2];  ///< 2 byte array to store current display content
volatile LED_OTHER_DEF      stDisp;            ///< bit field with led state and decimal point state

/**
 * Show characters on display, see @ref SEG_CHAR_ID_DEF
 * @param eChar1
 * @param eChar2
 */
void LED_Disp(SEG_CHAR_ID_DEF eChar1, SEG_CHAR_ID_DEF eChar2)
{
    acDispContent[0] = eChar1;
    acDispContent[1] = eChar2;
}

/**
 * Display 8bit value as 2 digit HEX on LED display
 * @param ucValue
 */
void LED_DispHex (unsigned char ucValue)
{
   LED_Disp (ucValue >> 4, ucValue & 0x0F);
}