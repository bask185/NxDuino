#include <Arduino.h>
#include "../../variables.h"

extern railItems getIO( uint8_t inputPin) ;
extern void storeIO( railItems* ) ;

extern void getSegment( trackSegments *segment, uint8_t _X, uint8_t _Y ) ;
extern void storeSegment( trackSegments *segment ) ;
uint8_t searchID( trackSegments*, uint8_t ) ;
extern void whipeEEPROM() ;