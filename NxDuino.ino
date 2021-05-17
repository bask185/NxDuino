#include "config.h"

#include "src/modules/eeprom.h"
#include "src/modules/mcp23017.h"
#include "src/modules/SDparser.h"

#ifdef XPRESSNET
#include "src/modules/XpressNet.h"
XpressNetClass XpressNet;
#endif

#include <Wire.h>

uint8_t stopLed, startLed ;

uint16_t iodir[8] = {0,0,0,0,0,0,0,0};
void loadEEPROM() 
{
    for( uint8_t element = 0; element < 128 ; element++ ) 
    {
        uint8_t xMcp = element / 16;
        uint8_t inputPin=  element % 16;
        
        railItems _IO = getIO( element ) ;
        // #ifndef XPRESSNET
        // sprintf(sbuf, "type %3d, ID %3d, inputPin %3d, outputPin %3d", _IO.type, _IO.ID, _IO.inputPin, _IO.outputPin ) ;
        // Serial.println(sbuf) ;
        // #endif

        // N.B all IO is an OUTPUT by default. Therefor we only need to set ioDir bits for INPUTS
        if( _IO.type == start_stop_sw/* && _IO.type  <= occupancy_2_I2C*/ )     // if device is I2C input,
        {            
            iodir[xMcp] |= ( 0x01 << inputPin)  ;                  // configure inputPinas input pullup
            Serial.println("switch found");
        }
    }

    for( uint8_t j = 0 ; j < nMcp ; j++ ) {                        // check if all slaves are present and set their IO dir registers
        Serial.print(iodir[j], BIN); Serial.print(' ');
        Serial.println(iodir[j], BIN);
        if( mcp[j].init(mcpBaseAddress + j , iodir[j]/*0x00FF*/ ))  {  		// if function returns true, the slave did NOT respond
            nMcp = j ;
            break ; 
        }
    }
    Debug( nMcp );
    Debug(F(" mcp23017 devices found\r\n" )) ;

    // something is wrong with EEPROM, so temporarily manually overwrite IODIR registers

    iodir[0] = 0x0300 ;
    iodir[1] = 0xFF00 ;
    mcp[0].init(mcpBaseAddress     , iodir[0] ) ;
    mcp[1].init(mcpBaseAddress + 1 , iodir[1] ) ;

    #ifndef XPRESSNET

    // for( uint16_t eeAddress = 0 ; eeAddress < 256 ; eeAddress += 4 )
    // {
    //     Wire.beginTransmission(0x50);
    //     Wire.write( HIGHBYTE(eeAddress) ) ;
    //     Wire.write(  LOWBYTE(eeAddress) ) ;
    //     Wire.endTransmission() ; 

    //     Wire.requestFrom( 0x50, 5 ) ;
    //     for(int i = 0 ; i < 4 ; i++ )
    //     {
    //         iodir[i] = Wire.read() ;
    //     } 
    //     sprintf( sbuf,"addr:%3d, ID:%3d, type:%3d, inputPin:%3d, outputPin:%3d",
    //             eeAddress ,
    //             iodir[0] ,
    //             iodir[1] ,
    //             iodir[2] ,
    //             iodir[3] ) ;
    //     Debug(sbuf) ;
    // }
    // for ( int x = 1 ; x <= 64 ; x ++ ) {
    //     for( int y = 1 ; y <= 32 ; y ++ ) {
    //         trackSegments segment ;
    //         getSegment( &segment, x, y ) ;
    //     }
    // }

    #endif
}

void readInputs()
{
    //uint8_t state;
    static uint16_t inputPrev[8] = {0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF} ;
    //static uint16_t inputPrev[8] = {0,0,0,0,0,0,0,0} ;

    for( uint8_t slave = 0 ; slave < nMcp ; slave ++ )                                                          // for all MCP23017 devices
    {
        uint16_t input = (( mcp[slave].getInput(portB) << 8 ) | mcp[slave].getInput(portA) ) | ~iodir[slave];   // read both I/O ports and OR them with iodir to discard leds

        for( uint8_t inputPin= 0 ; inputPin < 16 ; inputPin++ )                                                 // for all 16 I/O pins
        {   
            uint16_t newInput =             input & ( 1 << inputPin) ;                                          // get state of pin
            uint16_t oldInput =  inputPrev[slave] & ( 1 << inputPin) ;                                          // get previous state of same pin

            if( newInput != oldInput )                                                                          // and check if they differ
            {                    
                uint8_t state ;
                inputPrev[slave] = input ;
               
                uint8_t _IO = inputPin + slave * 16 + 1 ;                                           // calculate which IO has changed (0-127)
                if( input  & ( 1 << inputPin) ) state = 0 ;                                         // store the state of the changed I/O (not pressed = HIGH)
                else                            state = 1 ;

                Debug("\r\ninput ");
                Debug( _IO ) ; 
                Debug(F(" changed to "));
                Debug( state ) ;

                railItems obj = getIO( _IO ) ;

                sprintf( sbuf, "type:%3d, ID:%3d, inp:%3d, outp:%3d ", obj.type, obj.ID, obj.inputPin, obj.outputPin ) ;
                Debug(sbuf);



                if(obj.type == start_stop_sw )
                {
                    trackSegments segment ;

                    enum states {
                        waitButtonPress ,
                        waitOtherButton ,
                        searchRoute     ,
                    } ;

                    static uint8_t state_ = waitButtonPress ;
                    switch( state_ )
                    {
                    case waitButtonPress:
                        if( state ) // if button is pressed
                        {
                            uint8_t succes = searchID( &startSwitch, obj.ID ) ; // load the trackSegment which corresponds with the pressed button
                            if( succes ) 
                            {
                                state_ = waitOtherButton ;
                                startLed = obj.outputPin - 1 ;
                                Debug(F("Start switch pressed")) ;     // dump feedback on screen to check if this crap works or not
                                Debug( startSwitch.ID );
                                Debug( startSwitch.X );
                                Debug( startSwitch.Y );
                            }
                            else 
                            {
                                Debug(F("no switch found in EEPROM")) ;
                            }
                        }

                        break ;

                    case waitOtherButton:
                        if( !state )
                        {
                            state_ = waitButtonPress ;
                            Debug(F("Start switch released")) ;     // dump feedback on screen to check if this crap works or not
                        }
                        else
                        {
                            uint8_t succes = searchID( &stopSwitch, obj.ID ) ; // load the trackSegment which corresponds with the pressed button
                            if( succes ) 
                            {
                                state_ = searchRoute ;
                                stopLed = obj.outputPin - 1 ;
                                Debug(F("Stop switch also pressed")) ;     // dump feedback on screen to check if this crap works or not
                                Debug( stopSwitch.ID );
                                Debug( stopSwitch.X );
                                Debug( stopSwitch.Y );
                            }
                            else 
                            {
                                Debug(F("no switch found in EEPROM")) ;
                            }
                        }
                        break ;

                    case searchRoute:       // just set LEDs for now
                        if( !state )       // if any of the buttons is released...
                        {
                            mcpWrite( startLed, 1 ) ;
                            mcpWrite(  stopLed, 1 ) ;
                            state_ = waitButtonPress ;
                        }
                        break ;
                    }
                }
                //return ;
            }   
        } 
    } 
}

const int XNetAddress = 30 ;
const int XNetSRPin = 2 ;

void setup()
{
    Wire.begin() ;		// sets up I2C bus
    #ifdef XPRESSNET
    XpressNet.start( XNetAddress, XNetSRPin); // sets up XpressNet Bus
    #else
    Serial.begin(115200);
    #endif

    SDparser() ;		// checks if SD card with correct file is present, and if so,  whipes and fills the I2C EEPROM
    loadEEPROM() ;		// runs through EEPROM to initialize MCP23017 devices
    delay(100);

    //  for( int i = 1 ; i < 33 ; i ++ )
    //  {
    //      mcpWrite( i-1, 1 ) ;
    //      Debug(i) ;
    //      delay(1500) ;
    //      mcpWrite( i-1, 0 ) ;
    //      delay(500) ;
    //  }   
}

void loop()
{
    #ifdef XPRESSNET
    XpressNet.receive();
    #endif

    REPEAT_MS(20);

    readInputs() ;

    END_REPEAT

	//pathFinder()

}