#include "XpressNet.h"
#include "roundRobinTasks.h"
#include "variables.h"
#include <SD.h>
#include <SPI.h>


XpressNetClass XpressNet;   // xpressnet object
//File layout ;               // sd card file object

const int CSpin = 10 ;

//#define DEBUG
void debug( char * someTxt )
{
    #ifdef DEBUG
    Serial.print( someTxt ) ;
    #endif 
}

void debug( int num )
{
    #ifdef DEBUG
    Serial.print( num ) ;
    #endif 
}


void loadRailMap()
{
    //debug(F("\r\nloading rail map from SD card\r\n")) ;

    //if( SD.begin( CSpin) ) ; //debug(F("\r\nSD card initialized\r\n")) ;
//    else                    debug(F("\r\nWarning could not initialize SD card\r\n")) ;

   // layout = SD.open("layout.txt") ;
   // if( layout ); //debug(F("\r\nlayout.txt opened\r\n")) ;
   // else        /debug(F("\r\nwarning, could not open layout.txt\r\n")) ;

   // while( layout.available() ) {
     //   byte someSize = 1; 
     //   byte buffer[someSize] ;
      //  layout.readBytes( buffer, someSize ) ;
  //  }
    // debug(F("\r\nrail map loaded\r\n")) ;
}

void initMcp() 
{
    uint8_t ioDir[nMcp] ; // needs filling in
    //debug(F("\r\nintialising I2C bus\r\n")) ;
    for(byte j = 0 ; j < nMcp ; j++ ) {						// check if all 4 slaves are present and set their IO dir registers
		if( mcp[j].init(mcpBaseAddress + j , ioDir[j]) ) {  // if function returns true, the slave did NOT respond
			nMcp = j ;
			break ; 
		}
	}
   // debug( nMcp );
   // debug(F(" mcp23017 devices found\r\n" )) ;
}

const int XNetAddress = 30 ;
const int XNetSRPin = 2 ;
void initXpressnet() 
{
    XpressNet.start( XNetAddress, XNetSRPin);
}


void readInputs()
{
    uint8_t state;
    static uint16_t inputPrev[8] = {0,0,0,0,0,0,0,0} ;

    for( uint8_t slave = 0 ; slave < nMcp ; slave ++ )                                         // for all MCP23017 devices
    {
        uint16_t input = mcp[slave].getInput(portB) | (mcp[slave].getInput(portA) << 8);    // read both I/O ports

        for( uint8_t pin = 0 ; pin < 16 ; pin ++ )                                           // for all 16 I/O pins
        {   
            uint8_t newInp =             input & ( 1 << pin ) ;                             // get state of pin
            uint8_t oldInp =  inputPrev[slave] & ( 1 << pin ) ;                             // get previous state of same pin
            if( newInp != oldInp )                                                          // and check if they differ
            {    
                uint8_t state ;
                inputPrev[slave] = input ;
               
                uint8_t _IO = pin + slave * 16 ;                                                     // calculate which IO has changed
                if( input  & ( 1 << pin ) ) state = 0 ;                                     // store the state of the changed I/O (not pressed = HIGH)
                else                        state = 1 ;

                for( uint8_t i = 0 ; i < nRailItem ; i ++ )                                 // for loop to search which object is linked to this IO pin
                {
                    debug("\r\ninput ");
                    debug( _IO ) ; 
                    debug(F(" changed to "));
                    debug( state ) ;

                    uint8_t _type = IO[i].type ;
                    switch( _type )
                    {
                    case occupancy_1_I2C :                                                  // if any of these items -> update the state
                    case occupancy_2_I2C :
                    case start_stop_sw :
                        IO[i].type = state ;                                                // true or false
                        break ;
                    }
                }    
            } 
        } 
    } 
}

void updateOutputs()
{
    for( byte i = 0 ; i < nRailItem ; i ++ )
    {
        uint8_t state = IO[i].state ;

        if( state!= IO[i].statePrev )                                                // if state has changed
        {
            IO[i].statePrev = state;                                                    // safe change
 
            uint8_t _type  = IO[i].type ;
            if( _type >= route_led &&  _type <= uncoupler_I2C )                                       // checks if IO is an I2C output
            {
                uint8_t I2cPin =   IO[i].outputPin ; 
               // mcpWrite( I2cPin, state ) ; to be made
            }
            else if ( _type == point_DCC || _type == uncoupler_DCC )
            {
                uint8_t _ID = IO[i].ID ;
                XpressNet.setTrntPos( HIGHBYTE(_ID), LOWBYTE(_ID), state) ;
            }
        }
    }
}


void roundRobinTasksInit()
{
    loadRailMap() ;
    initMcp() ;
    initXpressnet() ;
}

void roundRobinTasks()
{
    XpressNet.receive();

    REPEAT_MS(20);

    readInputs() ;
    updateOutputs() ;

    END_REPEAT
}


