#include "eeprom.h"
#include <Wire.h>

#define hwAddress 0b1010000  // i2c address of eeprom provided all address pins are tied to gnd/0v

/*
* TRACKSEGMENTS
* EEPROM RANGE = 0x3FFF <> 0x7FFF
*/
#define segmentOffset 0x3FFF
void storeSegement( trackSegments segment ) 
{
	uint8_t nElements = sizeof( segment ) ;
	uint8_t *ptr;
	ptr = & segment.x0 ;							// pointer to first elemelent
	//			address = x * y + offset
	uint16_t eeAddress = (uint16_t)segment.x0 * (uint16_t)segment.y0 * (uint16_t)8 + segmentOffset ;     //extra casts because C and C++ is shite in this

	Wire.beginTransmission( hwAddress ) ;
	Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;

	for( int i = 0 ; i < nElements ; i ++ )
	{
		Wire.write( *ptr ) ;
		ptr ++ ;
	}
	Wire.endTransmission() ;
}

trackSegments getSegment( uint8_t X, uint8_t Y )
{
    trackSegments segment ;
    uint8_t *ptr ;                                        // declare pointer
    ptr = &segment.x0 ;
    uint8_t nElements = sizeof( segment ) ;

    X = constrain( X, 1, 64 ) ;                    // safety measure
    Y = constrain( Y, 1, 32 ) ;                    // safety measure
    X -- ;    
    Y -- ;

    uint16_t eeAddress = (uint16_t)X * (uint16_t)Y * (uint16_t)8 + segmentOffset ;     //extra casts because C and C++ is shite in this

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    Wire.endTransmission() ;
    Wire.requestFrom( hwAddress, nElements ) ;

    for( int i = 0 ; i < nElements ; i ++ )                // use for loop to initialize all bytes of local object before returning
    {
        *ptr = Wire.read() ;                            	// use pointer to scoop eeprom in struct obj
        ptr++ ;
    } 

    return segment ;
}


/*
* IO OBEJCTS
* EEPROM RANGE = 0x0000 <> 0x03FF
*/
void storeIO( railItems _IO ) 
{
	uint8_t nElements = sizeof( _IO ) ;
	uint8_t *ptr;
	ptr = & _IO.ID ;										// pointer to first elemelent
	//					 inputPin* 8 
	uint16_t eeAddress = _IO.inputPin* 8 ;    

	Wire.beginTransmission( hwAddress ) ;
	Wire.write( HIGHBYTE( eeAddress ) ) ;                
    Wire.write(  LOWBYTE( eeAddress ) ) ;

	for( int i = 0 ; i < nElements ; i ++ )
	{
		Wire.write( *ptr ) ;
		ptr ++ ;
	}
	Wire.endTransmission() ;
}

railItems getIO( uint8_t inputPin)
{
    railItems _IO ;                                        // declare local object called IO
    uint8_t *ptr ;                                        // declare pointer
    ptr = &_IO.ID ;                                      // let it point to first element of object

    inputPin= constrain( pin, 1, 128 ) ;                    // safety measure
    inputPin-- ;                                            // for 0 index misery

    uint16_t eeAddress = inputPin* 8 ;                        // should be always within 0-127

    Wire.beginTransmission( hwAddress ) ;
    Wire.write( HIGHBYTE( eeAddress ) ) ;
    Wire.write(  LOWBYTE( eeAddress ) ) ;
    Wire.endTransmission() ;
    Wire.requestFrom( hwAddress, sizeof( IO ) ) ;


    for( int i = 0 ; i < sizeof( IO ) ; i ++ )            // use for loop to initialize all bytes of local object before returning
    {
        *ptr = Wire.read() ;                            // use pointer to scoop eeprom in struct obj
        ptr++ ;
    } 

    return _IO ;                                        // return the IO object
}
