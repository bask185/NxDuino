#include <Arduino.h>
#include "src/modules/mcp23017.h"

#define REPEAT_MS(x)    { \
                            static uint32_t previousTime ; \
                            uint32_t currentTime = millis() ; \
                            if( currentTime - previousTime >= x ) \
                            {   \
                                previousTime = currentTime ;
                         
#define END_REPEAT          } \
                        }
#define LOWBYTE(x)   ((unsigned char) (x))
#define HIGHBYTE(x)  ((unsigned char) (((unsigned int) (x)) >> 8))

extern Mcp23017 mcp[] ;
extern uint8_t nMcp ;


const int nTrackSegment = 256 ; // 3 bytes each
const int nRailItem = 128 ;            

// all input and output types.
enum types
{
    start_stop_sw = 1,      // I2C input
    relay_sw ,              // I2C input
    occupancy_1_I2C ,       // I2C input
    occupancy_2_I2C ,       // I2C input
    occupancy_1_Xnet ,      // Xnet input message
    occupancy_2_Xnet ,      // Xnet input message
    point_pulse ,           // I2C output
    point_relay ,           // I2C output
    point_DCC ,             // Xnet/DCC output
    relay_I2C ,             // I2C output
    relay_DCC ,             // Xnet/DCC output
    line_ ,                 // just a line
    curve_ ,                // just a curve
    NA = 255                // not available = 255 
} ;

extern uint16_t outputs[ 8 ] ;

typedef struct someName {
    uint8_t ID ;
    uint8_t type ;
    uint8_t inputPin;           // some devices have both an input as well as an output such as a occupancy detector
    uint8_t outputPin ;
} railItems ;

extern railItems IO ;

// N.B. rail sections will propably be great in amount, therefor reduced RAM storage is preferable.
typedef struct  
{
    uint8_t X ;
    uint8_t Y  ; 
    uint8_t type ; // line (line, detector, button, whatever is straight), left point, right point, curve
    uint8_t dir ; 
} trackSegments ;

extern trackSegments trackSegment ;

