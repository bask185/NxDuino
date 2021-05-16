#include "config.h"

#include "src/modules/eeprom.h"
#include "src/modules/mcp23017.h"
#include "src/modules/SDparser.h"

#ifdef XPRESSNET
#include "src/modules/XpressNet.h"
XpressNetClass XpressNet;
#endif

#include <Wire.h>


void loadEEPROM() 
{
    uint16_t array[8] = {0,0,0,0,0,0,0,0};
    uint16_t *ioDir ;
    ioDir = &array[0] ;

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
        if( _IO.type >= start_stop_sw && _IO.type  <= occupancy_2_I2C )     // if device is I2C input,
        {
            Debug("setting input");
            ioDir += xMcp;                                     // match iodir's address to corresponding mcp23017
            (*ioDir) |= ( 0x01 << inputPin)  ;                  // configure inputPinas input pullup
            ioDir -= xMcp;                                     // set address back
        }
    }

    for( uint8_t j = 0 ; j < nMcp ; j++ ) {                        // check if all slaves are present and set their IO dir registers
        Serial.println( ioDir[j], BIN );
        if( mcp[j].init(mcpBaseAddress + j , /*ioDir[j]*/255 ))  {  		// if function returns true, the slave did NOT respond
            nMcp = j ;
            break ; 
        }
    }
    Debug( nMcp );
    Debug(F(" mcp23017 devices found\r\n" )) ;

    #ifndef XPRESSNET



    // for( uint16_t eeAdress = 0x3FFF ; eeAdress < 0x7FFF ; eeAdress += 5 )
    // {
    //     Wire.beginTransmission(0x50);
    //     Wire.write( HIGHBYTE(eeAdress) ) ;
    //     Wire.write(  LOWBYTE(eeAdress) ) ;
    //     Wire.endTransmission() ; 

    //     Wire.requestFrom( 0x50, 5 ) ;
    //     for(int i = 0 ; i < 5 ; i++ )
    //     {
    //         array[i] = Wire.read() ;
    //     } 
    //     sprintf( sbuf,"X:%3d, Y:%3d, ID:%3d, type:%3d dir:%3d",
    //             array[0] ,
    //             array[1] ,
    //             array[2] ,
    //             array[3] ,
    //             array[4] ) ;
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
    uint8_t state;
    static uint16_t inputPrev[8] = {255,255,255,255,255,255,255,255} ;

    for( uint8_t slave = 0 ; slave < nMcp ; slave ++ )                                              // for all MCP23017 devices
    {
        uint16_t input = ( mcp[slave].getInput(portB) << 8 ) | mcp[slave].getInput(portA);            // read both I/O ports

        for( uint8_t inputPin= 0 ; inputPin < 16 ; inputPin++ )                                     // for all 16 I/O pins
        {   
            uint8_t newInput =             input & ( 1 << inputPin) ;                               // get state of pin
            uint8_t oldInput =  inputPrev[slave] & ( 1 << inputPin) ;                               // get previous state of same pin
            if( newInput != oldInput )                                                              // and check if they differ
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
                Debug(F("matching output = ")) ;
                Debug(obj.outputPin - 1) ;

                if(obj.type == start_stop_sw )
                {
                    trackSegments segment ;
                    // Debug(F("search ID is")) ;
                    // Debug(obj.ID ) ;
                    // uint8_t succes = searchID( &segment, obj.ID ) ;
                    // if( succes ) {
                    //     Debug(F("Start/Stop switch pressed")) ;
                    //     Debug( segment.ID );
                    //     Debug( segment.X );
                    //     Debug( segment.Y );
                    // }
                    // else Debug(F("no switch found in EEPROM")) ;
                }
                mcpWrite( obj.outputPin - 2, state ) ;
                return ;
            }   
        } 
    } 
}


// tatic unsigned int inputPrev[8] = {0,0,0,0,0,0,0,0}, input = 0;
// 		input = mcp[slave].getInput(portB) | (mcp[slave].getInput(portA) << 8);	// read both I/O ports
// 		for(pin=0;pin<16;pin++) {												// for all 16 I/O pins
// 			if((input & (1 << pin)) != (inputPrev[slave] & (1 << pin))) {		// if an input (detector or memory) has changed...
// 				inputPrev[slave] = input;
// 				if(input & (1<<pin))	state = 0;					  			// store the state of the changed I/O (not pressed = HIGH)
// 				else					state = 1;
// 				IO = pin + slave * 16;											// calculate which IO has changed
// 				for(element=0; element<elementAmmount;

// void updateOutputs()
// {
//     for( byte i = 0 ; i < nRailItem ; i ++ )
//     {
//         uint8_t state = IO[i].state ;

//         if( state!= IO[i].statePrev )                                                   // if state has changed
//         {
//             IO[i].statePrev = state;                                                    // safe change
 
//             uint8_t _type  = IO[i].type ;
//             if( _type >= route_led &&  _type <= relay_I2C )                             // checks if IO is an I2C output
//             {
//                 uint8_t I2cPin =   IO[i].outputPin ; 
//                // mcpWrite( I2cPin, state ) ; to be made
//             }
//             else if ( _type == point_DCC || _type == relay_DCC )
//             {
//                 uint8_t _ID = IO[i].ID ;
//                 #ifdef XPRESSNET
//                 XpressNet.setTrntPos( HIGHBYTE(_ID), LOWBYTE(_ID), state) ;
//                 #endif
//             }
//         }
//     }
// }





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



//     for( int i = 1 ; i < 33 ; i ++ )
//     {
//         mcpWrite( i-1, 1 ) ;
//         Debug(i) ;
//         delay(1500) ;
//         mcpWrite( i-1, 0 ) ;
//         delay(500) ;
//     }   
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