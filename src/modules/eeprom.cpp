#include "eeprom.h"
#include <Wire.h>
#include "../../config.h"

#define hwAddress 0x50  // i2c address of eeprom provided all address pins are tied to gnd/0v

/*
* TRACKSEGMENTS
* EEPROM RANGE = 0x3FFF <> 0x7FFF

adress =   x * 5 + y

*/
#define segmentOffset 0x3FFF
#define segSize 5
#define ioSize 4
void storeSegment( trackSegments *segment ) 
{    //            address = x * y * 4 + offset
    uint16_t eeAddress = (uint16_t)(segment->X) * 160 + (segment->Y * segSize) + segmentOffset ;     //extra cast because C and C++ is shite in this

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    
    Wire.write( segment -> X ) ;
    Wire.write( segment -> Y ) ;
    Wire.write( segment -> ID ) ;
    Wire.write( segment -> type ) ;
    Wire.write( segment -> dir ) ;

    Wire.endTransmission() ;

    // sprintf( sbuf,"SAVE adr:%5d, X:%3d, Y:%3d, ID:%3d, type:%3d, dir:%3d",
    //     eeAddress ,
    //     segment -> X,
    //     segment -> Y,
    //     segment -> ID,
    //     segment -> type,
    //     segment -> dir) ;
    // Debug(sbuf);
}

void getSegment( trackSegments *segment, uint8_t _X, uint8_t _Y )
{
    _X = constrain( _X, 1, 64 ) ;                    // safety measure
    _Y = constrain( _Y, 1, 32 ) ;                    // safety measure
    _X -- ;    
    _Y -- ;

    uint16_t eeAddress = ((uint16_t)_X * 160)  + (_Y * segSize) + segmentOffset ;     //extra cast because C and C++ is shite in this

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    Wire.endTransmission() ;
    Wire.requestFrom( hwAddress, segSize ) ;

    segment->X     = Wire.read() ;
    segment->Y     = Wire.read() ;
    segment->ID    = Wire.read() ;
    segment->type  = Wire.read() ;
    segment->dir   = Wire.read() ;

    //sprintf( sbuf,"LOAD adr:%5d, X:%3d, Y:%3d", eeAddress, _X, _Y);
    // sprintf( sbuf,"LOAD adr:%5d, X:%3d, Y:%3d, ID:%3d, type:%3d, dir:%3d",
    //     eeAddress ,
    //     segment -> X,
    //     segment -> Y,
    //     segment -> ID,
    //     segment -> type,
    //     segment -> dir ) ;
    // Debug(sbuf);
}


/*
* IO OBEJCTS
* EEPROM RANGE = 0x0000 <> 0x03FF
*/
void storeIO( railItems* obj ) 
{
    uint16_t eeAddress = 0 ;
    if(      obj->inputPin  > 0 ) eeAddress =  obj->inputPin  * ioSize ;    
    else if( obj->outputPin > 0 ) eeAddress =  obj->outputPin * ioSize ;   

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

    uint16_t eeAddress = inputPin* ioSize ;                          // should be always within 0-127

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    Wire.endTransmission() ;
    Wire.requestFrom( hwAddress, ioSize ) ;


    for( int i = 0 ; i < sizeof( IO ) ; i ++ )                  // use for loop to initialize all bytes of local object before returning
    {
        *ptr = Wire.read() ;                                    // use pointer to scoop eeprom in struct obj
        ptr++ ;
    } 

    return _IO ;                                                // return the IO object
}

uint8_t searchID( trackSegments *segment, uint8_t searchID ) 
{
    for( uint16_t eeAddress = 0x3FFF ; eeAddress < 0x7FFF ; eeAddress += 5 )
    {
        Wire.beginTransmission( hwAddress ) ;
        Wire.write( HIGHBYTE( eeAddress ) ) ;                
        Wire.write(  LOWBYTE( eeAddress ) ) ;
        Wire.endTransmission() ;
        Wire.requestFrom( hwAddress, segSize ) ;

        segment->X     = Wire.read() ;
        segment->Y     = Wire.read() ;
        segment->ID    = Wire.read() ;
        segment->type  = Wire.read() ;
        segment->dir   = Wire.read() ;

        if(( segment->ID   == searchID )
        &&   segment->type == start_stop_sw )
        {
            return 1 ;  // report succes
        }
    }
    return 0 ; // report fail
}

void whipeEEPROM()
{
    Wire.beginTransmission( hwAddress ) ;
    Wire.write( 0 ) ; // adr 0
    Wire.write( 0 ) ; // adr 0
    for( uint16_t i = 0 ; i < 0x8000 ; i++ )
    {
        Wire.write( 255 );
    }
    Wire.endTransmission() ;
    Debug("EEPROM WIPED");
}