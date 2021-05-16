#include <SPI.h>
#include <SD.h>
#include "SDparser.h"
#include <Wire.h>
#include "../../config.h"
#include "eeprom.h"


const int chipSelect = 10;
// #define DEBUG
enum SDstates
{
    checkModule,
    checkFile,
    readByte,
    makeNumber, 
    storeElement ,
    storeObject ,
    discardCR ,
    storeString ,
    makingNumber , // last 2 are used to indicate which type of variable we are reading
    formingString ,
} ;

#define TRACK_SEGMENTS 1
#define IO 2


// const char* listOfObjects[] = {
//     "NOT DEFINED        " ,
//     "start_stop_sw      " ,
//     "relay_sw           " ,
//     "occupancy_1_I2C    " ,
//     "occupancy_2_I2C    " ,
//     "occupancy_1_Xnet   " ,
//     "occupancy_2_Xnet   " ,
//     "point_pulse        " ,
//     "point_relay        " ,
//     "point_DCC          " ,
//     "relay_I2C          " ,
//     "relay_DCC          " ,
//     "line_              " ,
//     "curve_             " 
// } ;


 #define DEBUG


void SDparser( )
{
    bool PIGS_CAN_FLY = true ;
    char c ;
    uint8_t number ;
    uint8_t mode = 0 ;
    uint8_t mode1 = 0 ;
    uint8_t type = 0 ;
    uint8_t array[8] = {1,0,0,0,0,0,0,0}; ;  // we can have 8 bytes per line
    uint8_t index = 0 ;
    String label = "" ;
    uint8_t state = checkModule ;
    File dataFile ;
    uint8_t firstNewline = 0 ;
    
    while( PIGS_CAN_FLY == true )
    {
        switch( state ) {
        case checkModule : // 0
            Debug(F("Initializing SD card...")); 

            if (!SD.begin( chipSelect )) {
                Debug(F("No card present"));
                return ;
            }
            Debug(F("card initialized"));
            state = checkFile ;
            break ;

        case checkFile : // 1
            dataFile = SD.open( "SD_DATA.TXT" );
            if( dataFile ) 
            {
                Debug("SD_DATA.TXT") ;
                Debug(" opened, whiping EEPROM") ;
                whipeEEPROM() ;
                state = readByte ;
            }
            else {
                Debug("SD_DATA.TXT" ) ;
                Debug(" not opened ;") ;
                return ;
            }
            break ;

        case readByte : // 2
            if( dataFile.available() > 0  ) 
            {
                c = dataFile.read() ;
            }
            else
            {
                dataFile.close() ;
                return ;
            }
                 if( c >= '0' && c <= '9' ) state = makeNumber ;
            else if( c >= 'A' && c <= '}' ) state = storeString  ;
            else if( c == ','  )            state = storeElement ;
            else if( c == '\n' )            state = storeObject ;
            else if( c == '\r' )            state = discardCR ;
            break ;

        case makeNumber :   // 3
            mode = makingNumber ;
            number *= 10 ;                  // shift old digits one to the left ;
            number += ( c - '0' ) ;         // add new digit and substract '0'
            state = readByte ;
            break ;

        case storeElement : // 4
            array[ index++ ] = number ;     // store number in array and increment index
            number = 0 ;
            state = readByte ;
            break ;

        case storeObject :  // 5
            // #ifdef DEBUG
            // sprintf(sbuf, "Storing number '%3d', on index '%1d'", number, index ) ;
            // Debug(sbuf);
            // #endif
            array[ index++ ] = number ;     // store number in array and increment index
            number = 0 ;

            if( label == "[TRACK_SEGMENTS]" )   { mode1 = TRACK_SEGMENTS ;   Debug(F("STORING TRACK_SEGMENTS")) ; }
            if( label == "[IO]" )               { mode1 = IO ;               Debug(F("STORING IO")) ; }

            if( mode1 == TRACK_SEGMENTS )
            {
                trackSegments segment ;
                segment.X   = array[0] ;
                segment.Y   = array[1] ;
                segment.ID  = array[2] ;
                segment.type= array[3] ;
                segment.dir = array[4] ;

                storeSegment( &segment ) ;
            }
            if( mode1 == IO) 
            {
                if( array[0] == 0 
                ||  array[1] == 0 
                || (array[2] == 0 && array[3] == 0) ) goto cleanUp ; // if either type or ID is 0, don't store it.

                railItems obj ;

                obj.ID          = array[0] ;
                obj.type        = array[1] ;
                obj.inputPin    = array[2] ;
                obj.outputPin   = array[3] ;
                storeIO( &obj );
            }
            
        cleanUp:
            number = 0 ;
            mode = 0 ;
            label = "" ;
            index = 0 ;
            for( int i = 0 ; i < 10 ; i ++ )
            {
                array[i] = 0 ;
            }
            state = readByte ;
            break ;

        case discardCR :    // '\r'
            state = readByte ;
            break ;

        case storeString : // any other char
            mode = formingString ;
            label += c ;
            state = readByte ;
            break ;
        }
    }
}