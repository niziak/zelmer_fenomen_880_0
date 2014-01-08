

/**
 * Hall sensors:
 *  - H1    constantly powered
 *  - H2    constantly powered
 *          when equipment is detected on H1 or H2,
 *          additional checking for H3 and HC is enabled by giving HI state on RC6 MCU port
 *
 *  - H3    powered from RC6 MCU port
 *
 *  - HC    hall sensor located on MCU PCB
 *          powered from RC6 MCU port
 *
 * Sensors and equipment mapping:
 *     8    4
 *  | H1 | H2 | check H3 & HC | H3 | HC | def.  | speed |
 *  |    |    |               |    |    | speed | range |
 *  +----+----+---------------+----+----+-------+-------+
 *  | X  |    |    check HC   |    |  X |  1    |       | ? potato
 *  | X  |    |    check H3   |  X |    |  12   | 6..12 | blender
 *  |    |  X |    check HC   |    |  X |  6    | 1..12 | slicer
 *  |    |  X |    check H3   |  X |    |  10   |       | juice maker
 *  | X  |  X |    check HC   |    |  X |       | 1..6  | mixer
 *
 *
 *
 */

/**
 * Connections between upper and lower PCBs:
 *  1) to MCU RC2 (active low)    - pulses from phototransistor (engine RPM)
 *  2) from MCU RC1 (active high) - pulses for triac gate
 *  3) to MCU RB0                 - square wave to detect zero crossing (also overheat detection)
 *  4) POWER (+)
 *  5) POWER (-)
 *  6) from MCU RA5 (active high) - enable engine relay
 * 
 *  7) to MCU RA0 (active low)    - signal for MCU that someone give ground to relay transistor.
 *     Now if MCU gives HI at RA5 (Conn 6), engine relay will be turned on.
 *      Possible sources of ground at PCB7 pin:
 *       - NPN transistor controlled from second PIC MCU (10Fxxx)
 *       - PNP transistor controlled from HC (HC gives LO signal when activated)
 *
 *
 *  8) from MCU RC6 (active high) - power for halls H3 and HC
 *
 *  9) to MCU RA1 (active low)    - Hall sensor H3 is active, thus PCB 7 is also low, so relay transistor is grounded.
 */

/**
 *                                   MCLR 01 #####   ##### 28 RB7 ICSP DAT ---> 7 seg
 *   [PCB 7]  ---------------------> RA0  02 ############# 27 RB6 ICSP CLD ---> 7 seg & 2 color LED Anode
 *   [PCB 9]  Hall 3 --------------> RA1  03 ############# 26 RB5 ------------> 7 seg
 *            Hall 2 --------------> RA2  04 ############# 25 RB4 ------------> 7 seg
 *            Hall 1 --------------> RA3  05 ############# 24 RB3 ------------> 7 seg & 2 color LED Anode
 * Upper key row(L when press) ----> RA4  06 ############# 23 RB2 ------------> 7 seg
 *           Relay <---------------- RA5  07 ############# 22 RB1 ------------> 7 seg
 *                               [-] Vss  08 ############# 21 RB0 INT <------- 0/5 V 50Hz cycle from mains (when absent - overheat)
 *                           XT ---- RA7  09 ############# 20 Vdd [+]
 *                           XT ---- RA6  10 ############# 19 Vss [-]
 *        2 color LED common K  <--- RC0  11 ############# 18 RC7 ------------> 7 seg dot
 *             Triac gate <---- CCP2 RC1  12 ############# 17 RC6 ------------> Vdd power for hall sensors: H3 and HC
 *             Enigne RPM ----> CCP1 RC2  13 ############# 16 RC5 ------------> 7 seg 1st display plus to Anode + [-] Key (active low)
 * Lower key row L when press) ----> RC3  14 ############# 15 RC4 ------------> 7 seg 2nd display plus to Anode + [+] Key (active low
 *
 * All hall sensors in idle gives 5V, when magnet is detected voltage drops to zero.
 *
 * Common anode of seven segments display is driven by PNP tranistor, so displays are activated using LOW state.
 *
 * Enigne RPM are 2 positive pulses per rotation. So it is good to measure time between rising edges.
 */


/**
 *
 */