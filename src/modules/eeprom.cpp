#include "eeprom.h"
#include <Wire.h>
#include "../../config.h"

#define hwAddress 0x50  // i2c address of eeprom provided all address pins are tied to gnd/0v

/*
* TRACKSEGMENTS
* EEPROM RANGE = 0x3FFF <> 0x7FFF
*/
#define segmentOffset 0x3FFF
void storeSegment( trackSegments *segment ) 
{    //            address = x * y * 4 + offset
    uint16_t eeAddress = (uint16_t) segment -> X * segment -> Y * 4 + segmentOffset ;     //extra cast because C and C++ is shite in this

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    
    Wire.write( segment -> X ) ;
    Wire.write( segment -> Y ) ;
    Wire.write( segment -> type ) ;
    Wire.write( segment -> dir ) ;

    Wire.endTransmission() ;

    #ifndef XPRESSNET
    sprintf( sbuf, "adr: %4d, X:%2d , Y:%2d, type:%2d, dir:%2d", eeAddress, segment->X, segment->Y, segment->type, segment->dir ) ;
    Debug( sbuf ) ;
    #endif
}

void getSegment( trackSegments *segment, uint8_t _X, uint8_t _Y )
{
    _X = constrain( _X, 1, 64 ) ;                    // safety measure
    _Y = constrain( _Y, 1, 32 ) ;                    // safety measure
    _X -- ;    
    _Y -- ;

    uint16_t eeAddress = (uint16_t)_X * _Y * 4 + segmentOffset ;     //extra cast because C and C++ is shite in this

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    Wire.endTransmission() ;
    Wire.requestFrom( hwAddress, 4 ) ;

    segment->X     = Wire.read() ;
    segment->Y     = Wire.read() ;
    segment->type  = Wire.read() ;
    segment->dir   = Wire.read() ;
}


/*
* IO OBEJCTS
* EEPROM RANGE = 0x0000 <> 0x03FF
*/
void storeIO( railItems* obj ) 
{
    uint16_t eeAddress = 0 ;
    if(      obj->inputPin  > 0 ) eeAddress =  obj->inputPin  * 4 ;    
    else if( obj->outputPin > 0 ) eeAddress =  obj->outputPin * 4 ;   
    Debug(eeAddress) ;
    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;

    Wire.write( obj->ID ) ;
    Wire.write( obj->type ) ;
    Wire.write( obj->inputPin ) ;
    Wire.write( obj->outputPin ) ;

    Wire.endTransmission() ;
}

railItems getIO( uint8_t inputPin)
{
    railItems _IO ;                                             // declare local object called IO
    uint8_t *ptr ;                                              // declare pointer
    ptr = &_IO.ID ;                                             // let it point to first element of object

    inputPin= constrain( inputPin, 1, 128 ) ;                        // safety measure
    inputPin -- ;                                               // for 0 index misery

    uint16_t eeAddress = inputPin* 4 ;                          // should be always within 0-127

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    Wire.endTransmission() ;
    Wire.requestFrom( hwAddress, 4 ) ;


    for( int i = 0 ; i < sizeof( IO ) ; i ++ )                  // use for loop to initialize all bytes of local object before returning
    {
        *ptr = Wire.read() ;                                    // use pointer to scoop eeprom in struct obj
        ptr++ ;
    } 

    return _IO ;                                                // return the IO object
}
