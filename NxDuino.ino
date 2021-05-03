//#include "NxDuino.h"
#include "roundRobinTasks.h"


void loadEEPROM(uint16_t *iodir){ // returns ammount of requied MCP23017 slaves depending on taught in IO
    uint16_t eeAddress;

    uint16_t ioDir[8] = {0,0,0,0,0,0,0,0};

    for( uint8_t element = 0; element < 128 ; element++ ) 
    {
        uint8_t xMcp = element / 16;
        uint8_t inputPin=  element % 16;
        
        railItems _IO = getIO( element ) ;

        // N.B all IO is an OUTPUT by default. Therefor we only need to set ioDir bits for INPUTS
        if( _IO.type >= start_stop_sw && _IO.type  <= occupancy_2_I2C )     // if device is I2C input,
        {
            iodir += xMcp;                                     // match iodir's address to corresponding mcp23017
            *iodir |= 0x01 << pin;                             // configure inputPinas input pullup
            iodir -= xMcp;                                     // set address back
        }
    }

    for( uint8_t j = 0 ; j < nMcp ; j++ ) {                        // check if all 4 slaves are present and set their IO dir registers
        if( mcp[j].init(mcpBaseAddress + j , ioDir[j]) ) {  // if function returns true, the slave did NOT respond
            nMcp = j ;
            break ; 
        }
    }
   // debug( nMcp );
   // debug(F(" mcp23017 devices found\r\n" )) ;
}


void readInputs()
{
    uint8_t state;
    static uint16_t inputPrev[8] = {0,0,0,0,0,0,0,0} ;

    for( uint8_t slave = 0 ; slave < nMcp ; slave ++ )                                         // for all MCP23017 devices
    {
        uint16_t input = mcp[slave].getInput(portB) | (mcp[slave].getInput(portA) << 8);    // read both I/O ports

        for( uint8_t inputPin= 0 ; inputPin< 16 ; inputPin++ )                                           // for all 16 I/O pins
        {   
            uint8_t newInp =             input & ( 1 << inputPin) ;                             // get state of pin
            uint8_t oldInp =  inputPrev[slave] & ( 1 << inputPin) ;                             // get previous state of same pin
            if( newInp != oldInp )                                                          // and check if they differ
            {    
                uint8_t state ;
                inputPrev[slave] = input ;
               
                uint8_t _IO = inputPin+ slave * 16 ;                                                     // calculate which IO has changed
                if( input  & ( 1 << inputPin) ) state = 0 ;                                     // store the state of the changed I/O (not pressed = HIGH)
                else                        state = 1 ;

                for( uint8_t i = 0 ; i < nRailItem ; i ++ )                                 // for loop to search which object is linked to this IO pin
                {
                    // debug("\r\ninput ");
                    // debug( _IO ) ; 
                    // debug(F(" changed to "));
                    // debug( state ) ;

                    uint8_t _type = 1; //IO[i].type ;   FIXME
                    switch( _type )
                    {
                    case occupancy_1_I2C :                                                  // if any of these items -> update the state
                    case occupancy_2_I2C :
                    case start_stop_sw :
                        //IO[i].type = state ;                                                // true or false immediate updated is needed
                        break ;
                    }
                }    
            } 
        } 
    } 
}

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
//                 XpressNet.setTrntPos( HIGHBYTE(_ID), LOWBYTE(_ID), state) ;
//             }
//         }
//     }
// }





const int XNetAddress = 30 ;
const int XNetSRPin = 2 ;

void setup()
{
	Wire.begin() ;		// sets up I2C bus
	XpressNet.start( XNetAddress, XNetSRPin); // sets up XpressNet Bus

    SDparser() ;		// checks if SD card with correct file is present, and if so,  whipes and fills the I2C EEPROM
	loadEEPROM() ;		// runs through EEPROM to initialize MCP23017 devices
	initMcp() ;

	//pathFinderInit() ;	// initialize pathFinder
	//Serial.begin(115200) ;
}

void loop()
{
	XpressNet.receive();

    REPEAT_MS(20);

    readInputs() ;
    //updateOutputs() ;

    END_REPEAT

	pathFinder()

}