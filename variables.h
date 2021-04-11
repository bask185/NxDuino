#include <Arduino.h>
#include "src/modules/mcp23017.h"

extern Mcp23017 mcp[] ;
extern uint8_t nMcp ;

const int nTrackSegment = 256 ; // 3 bytes each
const int nRailItem = 128 ;			

// all input and output types.
enum types {
    start_stop_sw           ,   // I2C input
    uncoupler_sw            ,   // I2C input
    occupancy_1_I2C         ,   // I2C input
    occupancy_2_I2C         ,   // I2C input
    route_led               ,   // I2C output
    occupance_led_1         ,   // I2C output
    occupance_led_2         ,   // I2C output
    point_pulse           	,   // I2C output
    point_relay           	,   // I2C output
    uncoupler_I2C           ,   // I2C output
    point_DCC             	,	// Xnet/DCC
    uncoupler_DCC          	,	// Xnet/DCC
    occupancy_1_Xnet        ,	// Xnet
    occupancy_2_Xnet        ,	// Xnet
} ;
types  type ;


typedef struct someName {
    uint8_t ID ;
    uint8_t type ;
    uint8_t state ;
    uint8_t statePrev ;
    uint8_t inputPin ;      // some devices have both an input as well as an output such as a occupancy detector
    uint8_t outputPin ;
    uint8_t x  ;
    uint8_t y  ; 
	uint8_t dir ;
    // uint8_t x1 ; // 'normal' objects have 2 connections
    // uint8_t y1 ; 
    // uint8_t x2 ;
    // uint8_t y2 ;
    // uint8_t x3 ; // points have 3 connections <-- node only
    // uint8_t y3 ;
} railItems ;
extern railItems IO[nRailItem] ;

// N.B. rail sections will propably be great in amount, therefor reduced RAM storage is preferable.
typedef struct  {
    //uint8_t ID ;
    //uint8_t type ;
    //uint8_t pin ;
    uint8_t X ;
    uint8_t Y  ; 
	uint8_t dir ;
    //uint8_t x1 ; // 'normal' objects have 2 connections
    //uint8_t y1 ; 
    //uint8_t x2 ;
    //uint8_t y2 ;
    //uint8_t x3 ; // points have 3 connections <-- node only
    //uint8_t y3 ;
} trackSegments ;
extern trackSegments track[] ;

