// HEADER FILES
#include <Arduino.h>
#include "NxDuino.h"
#include "serial.h"
#include "src/basics/timers.h"
#include "src/basics/io.h"

// MACROS
#define stateFunction(x) static bool x##F(void)
#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else
#define State(x) break; case x: if(runOnce) Serial.println(#x); if(x##F())
#define STATE_MACHINE_BEGIN if(!enabled) { \
    if(!NxDuinoT) enabled = true; } \
else switch(state){\
    default: Serial.println("unknown state executed, state is idle now"); state = NxDuinoIDLE; case NxDuinoIDLE: return true;
#define STATE_MACHINE_END break;}return false;


#define beginState initNXduino
#ifndef beginState
#error beginState not yet defined
#endif

// VARIABLES
static unsigned char state = beginState;
static bool enabled = true, runOnce = true, exitFlag = false;

uint8_t *startX, *startY, *stopX, *stopY, *startPin, *stopPin ;

typedef struct Objects {
    uint8_t ID ;
    uint8_t type ;
    uint8_t pin ;
    uint8_t x1 ; // 'normal' objects have 2 connections
    uint8_t y1 ; 
    uint8_t x2 ;
    uint8_t y2 ;
    uint8_t x3 ; // points have 3 connections <-- node only
    uint8_t y3 ;
} Object ;

Object current, adjacent, start, stop ;

#define SD

// FUNCTIONS
extern void NxDuinoInit(void)
{
    state = beginState ;
}

// This functions reads in all listed buttons and returns true when both a start button
uint8_t readButtonStates(uint8_t *_first, uint8 *_second)
{
    static uint8_t first = 0xFF ;
    
    for( int i = 0 ; i < nButtons ; i ++ )
    {
        uint8_t state = mcpRead( btnArray[i] ) ;
        if( state )
        {                                           // if button is pressed...
            if( first == 0xFF )                     // if first spoke is not yet pressed
            {
                first = i ;                         // flag that first spoke is found
                debug(F("first button pressed"));
            }
            else                                    // if first spoke is already recorded, record second button and signal true ;
            {
                *_first = first ;                   // pass numbers to calling routine
                *_second = i ;
                debug(F("second button pressed, READY"));
                
                first = 0xFF ;                      // reset first button for next time.
                return 1 ;                          // flag ready
            }
        } else if ( i == *first ) {                 // if the first button is no longer pressed, reset it
            first = 0xFF ;
            debug(F("first button released"));
        }
    }
    
    return 0 ;
}

Object loadNextObject()
{ 
    Object localObj ;
    uint8_t* pointer = (uint8_t*) &localObj;
    char[] text = "" ;
    
    while( file.available() )
    {
        uint8_t b = file.read()
        
        if( b ==  ',' || b == '\n') {
            *pointer = text.toInt() ;           // parse string to integer and stuff it in localObj
            text = "" ;                         // empty the string
            
            pointer ++ ;                        // and point to next element in localObj
            
            if( b == '\n' ) return localObj ;   // if newline character, return object
        }
        else
        {
            text += b ;                         // if neithe ',' or '\n' than apped character to string
        }
    }
}

void pushNode ( uint8_t *Xpos, uint8_t *Ypos )
{
    nodeArray[ nodeIndex++ ] = *Xpos ;
    nodeArray[ nodeIndex++ ] = *Ypos ;
}

void popNode( uint8_t *Xpos, uint8_t *Ypos )
{
    *Ypos = nodeArray[ --nodeIndex ] ;
    *Ypos = nodeArray[ --nodeIndex ] ;
}




extern byte NxDuinoGetState(void)
{
    return state ;
}

extern void NxDuinoSetState(unsigned char _state)
{
    state = _state ;
    runOnce = true ;
}

static void nextState(unsigned char _state, unsigned char _interval)
{
    runOnce = true ;
    exitFlag = false ;
    if(_interval)
    {
        enabled = false;
        NxDuinoT = _interval ;
    }
    state = _state ;
}

// STATE FUNCTIONS

stateFunction(initNXduino) {
    entryState
    {
        
    }
    onState
    {

        exitFlag = true;
    }
    exitState
    {

        return true;
    }
}
/*
    currentX/Y is where we are now
    newX/Y is where we are going to look for a new object
    X1/Y1 or X2/Y2 must be pointing to currentX/Y
*/
stateFunction(getButtons) {
    uint8_t startPin, stopPin ;
    
    entryState
    {
        
    }
    onState
    {
        if( readButtonStates( &startPin, &stopPin ) )           // returns true when 2 buttons are pressed. 
        {
            startObj = getStartObject( startPin ) ;             // retreive start coordinates
            stopObj  =  getStopObject(  stopPin ) ;             // retreive   end coordinates
            
            current = startObj ;                             // start object is a node 
            push( current.X, current.Y ) ;
            
            exitFlag = true;
        }
    }
    exitState
    {
        return true;
    }
}

stateFunction(findAdjacentObject) {
    uint8_t searchX, searchY ;
    
    entryState
    {
        type = nothing ;
        switch( connectedSide ) {                                       // record
        case 1: searchX = current.X1 ; searchY = current.Y1 ; break ;
        case 2: searchX = current.X2 ; searchY = current.Y2 ; break ;
        case 3: searchX = current.X3 ; searchY = current.Y3 ; break ;
        }
    }
    onState
    {        
        for( int i = 0 ; i < nObjects ; i ++ )
        {
            adjacent = loadNextObject();
            
            if( searchX == adjacent.X && searchY == adjacent.Y )       // if adjacent object is within our search coordinates.
            {                                                          // than we need to check if the adjacent object is connected to us as well
                if( current.X == adjacent.X1 
                &&  current.Y == adjacent.Y1 )
                {
                    connectedSide = 1;                              // important so we know on which side of this object we need to look.
                    type = adjacent.type ;                          // can be: node, led, wall or other NOTE. IS type needed????
                }
                else
                if( current.X == adjacent.X2 
                &&  current.Y == adjacent.Y2 )
                {
                    connectedSide = 2;
                    type = adjacent.type ; 
                }
                else
                if( current.X == adjacent.X3                // POINT ONLY
                &&  current.Y == adjacent.Y3 )
                {
                    connectedSide = 3;
                    type = adjacent.type ;                       // can only be a point
                }
            break ;         
            }
        }
        
        debug(F("Found type :")) ;
        #define printDebug(x) case x: debug(F(#x)) ; break ; // not sure if F macro inside another macro works?
        switch(type)
        {
            printDebug( wall ) ;
            printDebug( node ) ;
            printDebug( endObj ) ;
            printDebug( led ) ;
        }
        #undef printDebug

        exitFlag = true;
    }
    exitState
    {
        if( type != NA) current = adjacent ;                  // copy adjacent obj to current obj
        
        return true;
    }
}


stateFunction(storeNode) {
    entryState
    {
        push( current.X, current.Y ) ;                           // push node coordinates on the node stack
    }
    onState
    {
        exitFlag = true;
    }
    exitState
    {
        return true;
    }
}

stateFunction(loadNode) {
    uint8_t X, Y ;
    
    entryState
    {
        pop( &X, &Y ) ;                                         // pop node coordinates from the node stack
        loadNode( &current, X, Y);
        whipeLedArray() ;                                       // no route found at this point so whipe the LED array
    }
    onState
    {
        exitFlag = true;
    }
    exitState
    {

        return true;
    }
}

stateFunction(fail) {
    entryState
    {
        
    }
    onState
    {
        if( blink( errorLed ) )
        {
            exitFlag = true;
        }
    }
    exitState
    {

        return true;
    }
}

stateFunction(succes) {
    entryState
    {
    }
    onState
    {
        if( blink( succesLed ) )
        {
            exitFlag = true;
        }
    }
    exitState
    {

        return true;
    }
}

stateFunction(storeLed) {
    entryState
    {
        
    }
    onState
    {
        push( led, pin ) ;

        exitFlag = true;
    }
    exitState
    {

        return true;
    }
}

// STATE MACHINE
extern bool NxDuino(void) {
    STATE_MACHINE_BEGIN
    
    
    State(initNXduino) {
        nextState(getButtons, 0); }

    State(getButtons) {
        nextState(findAdjacentObject, 0); }

    State(findAdjacentObject) {
        if( type == node )      nextState(storeNode, 0);
        if( type == other )     nextState(findAdjacentObject, 0);
        if( type == nothing ) {
            if( nodesLeft )     nextState(loadNode, 0);
            else                nextState(fail, 0); }
        if( type == endButton)  nextState(succes, 0);
        if( type == led )       nextState(storeLed, 0); }

    State(storeNode) {
        nextState(findAdjacentObject, 0); }

    State(loadNode) {
        nextState(findAdjacentObject, 0); }

    State(fail) {
        nextState(getButtons, 0); }

    State(succes) {
        nextState(getButtons, 0); }

    State(storeLed) {
        nextState(findAdjacentObject, 0); }

    STATE_MACHINE_END
}
