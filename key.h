/* 
 * File:   key.h
 * Author: nizinski_w
 *
 * Created on 12 grudzień 2013, 10:53
 */

#ifndef KEY_H
#define	KEY_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
    KEY_NONE    = 0x3F,         ///< 0011 1111  - no key pressed, all key bits are 1

    KEY_MINUS   = 0x3E,         ///< 0011 1110
    KEY_PULSE   = 0x3D,         ///< 0011 1101
    KEY_PLUS    = 0x3B,         ///< 0011 1011
    KEY_AUTO    = 0x37,         ///< 0011 0111
    KEY_TURBO   = 0x2F,         ///< 0010 1111
    KEY_START   = 0x1F,         ///< 0001 1111

    KEY_LAST    = 0xFF,         // to force at least 8 bit enum
} KEY_DEFS;

extern inline KEY_DEFS eGetKey(void);
extern inline void vScanKeyAndHandle(void);


#ifdef	__cplusplus
}
#endif

#endif	/* KEY_H */

