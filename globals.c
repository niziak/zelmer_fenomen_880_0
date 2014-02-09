
volatile unsigned char   bEngineOn;                 ///< enable engine power and disable engine brake
volatile unsigned char   bTriacOn;                  ///< permits EINT and CCP2 module generate gating pulses
volatile unsigned char   bOverheat;                 ///< engine overheat
volatile unsigned char   bWrongEq;                  ///< wrong equipment (sensors broken?)
volatile unsigned char   bGoodEq;                   ///< equipment are valid and close (possible to turn on engine)
volatile unsigned char   ucSelectedEngineSpeed;     ///< variable to keep user selected speed 0..12
volatile unsigned int    uiCurrentTriacDelayInT1;   ///< current triac pulse delay from ZC in ticks of T1 timer

volatile unsigned int    uiTachoCurrentValue;       ///< current tacho value
volatile unsigned int    uiTachoLockValue;          ///< locked (requested) tacho value

volatile unsigned int    uiOnePulseCountdown;       ///<
volatile unsigned char   bMainTrigger;              ///< T0 trigger for main loop (active when 0)
volatile unsigned char   bNewTachoValue;            ///< flag to signal new read from tacho
