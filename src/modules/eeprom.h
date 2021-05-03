#include <Arduino.h>
#include "../../variables.h"

extern railItems getIO( uint8_t inputPin) ;
extern void storeIO( railItems _IO ) ;

extern trackSegments getSegment( uint8_t X, uint8_t Y ) ;
extern void storeSegement( trackSegments segment ) ;