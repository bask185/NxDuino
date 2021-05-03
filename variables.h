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
enum types {
    start_stop_sw           ,   // I2C input
    relay_sw                ,   // I2C input
    occupancy_1_I2C         ,   // I2C input
    occupancy_2_I2C         ,   // I2C input
    route_led               ,   // I2C output
    occupance_led_1         ,   // I2C output
    occupance_led_2         ,   // I2C output
    point_pulse             ,   // I2C output
    point_relay             ,   // I2C output
    relay_I2C               ,   // I2C output
    relay_DCC               ,   // Xnet/DCC output
    point_DCC               ,   // Xnet/DCC output
    occupancy_1_Xnet        ,   // Xnet input message
    occupancy_2_Xnet        ,   // Xnet input message
    NA = 255                    // not available = 255 
} ;
//types  type ;


typedef struct someName {
    uint8_t ID ;
    uint8_t type ;
    uint8_t state ;
    uint8_t statePrev ;
    uint8_t inputPin;           // some devices have both an input as well as an output such as a occupancy detector
    uint8_t outputPin ;
} railItems ;
extern railItems IO ;

// N.B. rail sections will propably be great in amount, therefor reduced RAM storage is preferable.
typedef struct  
{
    uint8_t x0 ;
    uint8_t y0  ; 
    uint8_t type ; // line (line, detector, button, whatever is straight), left point, right point, curve
    uint8_t dir ; 
    uint8_t x2 ;    // not in use
    uint8_t y2 ;    // not in use
    uint8_t x3 ;    // not in use
    uint8_t y3 ;    // not in use
} trackSegments ;
extern trackSegments trackSegment ;

