
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



volatile SEG_CHAR_ID_DEF acDispContent[2];  ///< 2 byte array to store current display content


