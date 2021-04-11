#include "roundRobinTasks.h"
#include "variables.h"

uint8_t taskCounter = 0 ;

void roundRobinTasksInit()
{
    // TODO 
    // load railmap + IO
    // init mcp devices    
}

void readI2cInputs()
{
    uint8_t state;
    static uint16_t inputPrev[8] = {0,0,0,0,0,0,0,0} ;

    for( byte slave = 0 ; slave < nMcp ; slave+ + )                                         // for all MCP23017 devices
    {
        uint16_t input = mcp[slave].getInput(portB) | (mcp[slave].getInput(portA) << 8);    // read both I/O ports

        for( uint8_t in = 0 ; pin < 16 ; pin ++ )                                           // for all 16 I/O pins
        {   
            uint8_t newInp =             input & ( 1 << pin ) ;                             // get state of pin
            uint8_t oldInp =  inputPrev[slave] & ( 1 << pin ) ;                             // get previous state of same pin
            if( newInp != oldInp )                                                          // and check if they differ
            {    
                inputPrev[slave] = input ;
               
                IO = pin + slave * 16 ;                                                     // calculate which IO has changed
                if( input  & ( 1 << pin ) ) state = 0 ;                                     // store the state of the changed I/O (not pressed = HIGH)
                else                        state = 1 ;

                for( uint8_t i = 0 ; i < nRailItem ; i ++ )                                 // for loop to search which object is linked to this IO pin
                {
                    uint8_t type = IO[i].type ;
                    switch( type )
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
    for( byte i = 0 ; i < nElements ; i ++ )
    {
        if( railItem[i].state != railItem[i].statePrev )                                                // if state has changed
        {
            railItem[i].statePrev = railItem[i].state ;                                                    // safe change

            uint8_t type = railItem[i].type ;
            if( type >= route_led &&  type <= uncoupler_I2C )                                       // checks if railItem is an I2C output
            {
                uint8_t I2cPin =   railItem[i].pin ; 
                uint8_t I2cState = railItem[i].state ; 
                mcpWrite( I2cPin, I2cState ) ;
            }
            else if ( type == point_DCC || type == uncoupler_DCC )
            {
                // Xnet.transmitt( ID, state, ) 
            }
        }
    }
}

void readXnet()
{

}


void roundRobinTasks()
{
    switch( ++ taskCounter )
    {
    default: taskCounter = 0 ;

    case 0:
        readI2cInputs() ;
        break ;

    case 1:
        updateOutputs() ;
        break ;

    case 2:
        readXnet() ;
        break ;
    }
}


