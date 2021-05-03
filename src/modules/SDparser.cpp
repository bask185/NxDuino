#include <SPI.h>
#include <SD.h>
#include "SDparser.h"
#include <Wire.h>

const int chipSelect = 10;

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

 //#define DEBUG
void Debug(String txt) 
{
    #ifdef DEBUG
    Serial.println(txt);
    #endif
}
void Debug(char* txt) 
{
    #ifdef DEBUG
    Serial.println(txt);
    #endif
}

#ifdef DEBUG
const char* listOfObjects[] = {
    "NOT DEFINED        " ,
    "start_stop_sw      " ,
    "relay_sw           " ,
    "occupancy_1_I2C    " ,
    "occupancy_2_I2C    " ,
    "occupancy_1_Xnet   " ,
    "occupancy_2_Xnet   " ,
    "point_pulse        " ,
    "point_relay        " ,
    "point_DCC          " ,
    "relay_I2C          " ,
    "relay_DCC          " ,
    "line_              " ,
    "curve_             " 
} ;
#endif

void storeSegment( uint16_t eeAddress, uint8_t type, uint8_t dir )
{
    Wire.beginTransmission( 0x50 ) ;
    Wire.write( eeAddress >> 8 ) ;  // HB
    Wire.write( eeAddress ) ;       // LB
    Wire.write( type ) ;
    Wire.write( dir ) ;
    Wire.endTransmission() ;
}

void storeIO( uint8_t ID, uint8_t type, uint8_t inp, uint8_t outp ) ;
{
    uint16_t eeAddress = 0x3FFF ;   // offset halfway the EEPROM

    if( inp != 0 )  eeAddress += (4 *  inp) ;        // if input is preset, use for the address
    else            eeAddress += (4 * outp) ;        // otherwise use the output pin for the address

    Wire.beginTransmission( 0x50 ) ;
    Wire.write( eeAddress >> 8 ) ;  // HB
    Wire.write( eeAddress ) ;       // LB
    Wire.write( ID ) ;
    Wire.write( type ) ;
    Wire.write( inp ) ;
    Wire.write( outp ) ;
    Wire.endTransmission() ;
}


void SDparser( )
{
    bool PIGS_CAN_FLY = true ;
    char c ;
    uint8_t number ;
    uint8_t mode = 0 ;
    uint8_t mode1 = 0 ;
    uint8_t type = 0 ;
    uint8_t array[10] = {1,0,0,0,0,0,0,0,0,0}; ;  // we can have 8 bytes per line
    uint8_t index = 0 ;
    uint8_t sbuf[50] ;  // reserve 50 chars to use with sprintF
    String label = "" ;
    uint8_t state = checkModule ;
    File dataFile ;
    uint8_t firstNewline = 0 ;
    
    while( PIGS_CAN_FLY == true )
    {
        switch( state ) {
        case checkModule : // 0
            //Debug(F("Initializing SD card...")); 

            if (!SD.begin( chipSelect )) {
                //Debug(F("No card present"));
                return ;
            }
            //Debug(F("card initialized"));
            state = checkFile ;
            break ;

        case checkFile : // 1
            dataFile = SD.open( "SD_DATA.TXT" );
            if( dataFile ) 
            {
                //Debug(fileName) ;
                //Debug(" opened ;") ;
                state = readByte ;
            }
            else {
                //Debug(fileName ) ;
                //Debug(" not opened ;") ;
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
            #ifdef DEBUG
            // sprintf(sbuf, "Storing number '%3d', on index '%1d'", number, index ) ;
            // Debug(sbuf);
            #endif
            array[ index++ ] = number ;     // store number in array and increment index
            number = 0 ;
            state = readByte ;
            break ;

        case storeObject :  // 5
            #ifdef DEBUG
            // sprintf(sbuf, "Storing number '%3d', on index '%1d'", number, index ) ;
            // Debug(sbuf);
            #endif
            array[ index++ ] = number ;     // store number in array and increment index
            number = 0 ;

            if( label == "[TRACK_SEGMENTS]" )   { mode1 = TRACK_SEGMENTS ;   /*Debug(F("STORING TRACK_SEGMENTS")) ;*/ }
            if( label == "[IO]" )               { mode1 = IO ;               /*Debug(F("STORING IO")) ; */}

            if( mode1 == TRACK_SEGMENTS )
            { // data format: X, Y, type, direction (bit 7=1 right point, bit 7=0 left point)
                if( array[2] == 0 ) goto cleanUp ; // if type is 0, something is wrong, do not store it
                #ifdef DEBUG
                sprintf(sbuf,"X =%3d, Y =%3d, dir =%3d, type = %s,", array[0], array[1], array[3], listOfObjects[array[2]] ) ;
                Debug( sbuf ) ; 
                #endif
                // eeprom address = 32 * X + Y. Xmax = 63, Ymax = 31 -> max address is 2048
                uint16_t X   = array[0] ; // cast to 16 bit instead of 8 to prevent possibly calculation issues with the address
                uint16_t Y   = array[1] ;
                uint8_t type = array[2] ;
                uint8_t dir  = array[3] ;
                uint16_t eeAddress = (64 * X) + (2 * Y) ;
                storeSegment( eeAddress, type, dir ) ;
            }
            if( mode1 == IO) 
            {
                if( array[0] == 0 
                ||  array[1] == 0 
                || (array[2] == 0 && array[3] == 0) ) goto cleanUp ; // if either type or ID is 0, don't store it.

                #ifdef DEBUG
                sprintf(sbuf,"ID =%3d, type = %s, inputPin =%3d, outputPin = %3d,", array[0], listOfObjects[array[1]], array[2], array[3]  ) ;
                Debug( sbuf ) ; 
                #endif
                
                uint8_t ID   = array[0] ;
                uint8_t type = array[1] ;
                uint8_t inp  = array[2] ;
                uint8_t outp = array[3] ;

                storeIO( ID, type, inp, outp ) ;
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

/* was used for the test 
void setup()
{
    Serial.begin( 115200 ) ;
    SDparser() ;
}

void loop()
{
}

*/
