// // HEADER FILES
// #include <Arduino.h>
// #include "pathfinder.h"
// #include "config.h"
// //#include "serial.h"
// //#include "src/basics/timers.h"
// //#include "src/basics/io.h"

// // MACROS
// #define stateFunction(x) static bool x##F(void)
// #define entryState if(runOnce) 
// #define onState runOnce = false; if(!runOnce)
// #define exitState if(!exitFlag) return false; else
// #define State(x) break; case x: if(x##F())
// #define STATE_MACHINE_BEGIN if(!enabled) { \
//     if(!NxDuinoT) enabled = true; } \
// else switch(state){\
//     default: /*Serial.println("unknown state executed, state is idle now");*/ state = NxDuinoIDLE; case NxDuinoIDLE: return true;
// #define STATE_MACHINE_END break;}return false;


// #define beginState initNXduino
// #ifndef beginState
// #error beginState not yet defined
// #endif

// // VARIABLES
// static unsigned char state = beginState, type, nodesLeft ;
// static bool enabled = true, runOnce = true, exitFlag = false;


// uint8_t *startX, *startY, *stopX, *stopY, *startPin, *stopPin ;

// enum types {
//     node,
//     other,
//     nothing,
//     endButton
// }

// const int offsets[4][8][8] = 
// { 
//     {//dir 1   2   3   4   5   6   7   8    
//         {  0,  1,  1,  1, -1, -1, -1,  1 } ,        // curve
//         {  1,  1,  0, -1,  0, -1,  0, -1 } , 
//         { -1,  0,  1,  1,  1,  0, -1, -1 } , 
//         { -1, -1, -1,  0,  1,  1,  1,  0 } , 
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA 
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA 
//         {  0,  0,  0,  0,  0,  0,  0,  0 }          // NA
//     } ,
//     {
//         {  1,  1,  0, -1, -1, -1,  0,  1 } ,        // line (and most other objects)
//         {  0,  1,  1,  1,  0, -1, -1, -1 } ,
//         { -1, -1,  0,  1,  1,  1,  0, -1 } ,
//         {  0, -1, -1, -1,  0,  1,  1,  1 } ,
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA 
//         {  0,  0,  0,  0,  0,  0,  0,  0 }          // NA
//     } ,
//     {
//         {  1,  1,  0, -1, -1, -1,  0,  1 } ,        // switch left
//         {  0,  1,  1,  1,  0, -1, -1, -1 } ,
//         { -1, -1,  0,  1,  1,  1,  0, -1 } ,
//         {  0, -1, -1, -1,  0,  1,  1,  1 } ,
//         { -1, -1, -1,  0,  1,  1,  1,  0 } ,
//         {  1,  0, -1, -1, -1,  0,  1,  1 } ,
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // might need filling in 
//         {  0,  0,  0,  0,  0,  0,  0,  0 }          // might need filling in 
//     } ,
//     {
//         {  1,  1,  0, -1, -1, -1,  0,  1 } ,        // switch right
//         {  0,  1,  1,  1,  0, -1, -1, -1 } ,
//         { -1, -1,  0,  1,  1,  1,  0, -1 } ,
//         {  0, -1, -1, -1,  0,  1,  1,  1 } ,
//         { -1,  0,  1,  1,  1,  0, -1, -1 } ,
//         {  1, -1, -1,  0,  1, -1,  1,  0 } ,
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // might need filling in 
//         {  0,  0,  0,  0,  0,  0,  0,  0 }          // might need filling in 
//     }
// } ;

// #define SD

// // // FUNCTIONS
// // extern void NxDuinoInit(void)
// // {
// //     state = beginState ;
// // }


// // Object loadNextObject()
// // { 
// //     Object localObj ;
// //     uint8_t* pointer = (uint8_t*) &localObj;
// //     char[] text = "" ;
    
// //     while( file.available() )
// //     {
// //         uint8_t b = file.read()
        
// //         if( b ==  ',' || b == '\n') {
// //             *pointer = text.toInt() ;           // parse string to integer and stuff it in localObj
// //             text = "" ;                         // empty the string
            
// //             pointer ++ ;                        // and point to next element in localObj
            
// //             if( b == '\n' ) return localObj ;   // if newline character, return object
// //         }
// //         else
// //         {
// //             text += b ;                         // if neithe ',' or '\n' than apped character to string
// //         }
// //     }
// // }

// void pushNode ( uint8_t *Xpos, uint8_t *Ypos )
// {
//     nodeArray[ nodeIndex++ ] = *Xpos ;
//     nodeArray[ nodeIndex++ ] = *Ypos ;
// }

// void popNode( uint8_t *Xpos, uint8_t *Ypos )
// {
//     *Ypos = nodeArray[ --nodeIndex ] ;
//     *Ypos = nodeArray[ --nodeIndex ] ;
// }




// extern byte NxDuinoGetState(void)
// {
//     return state ;
// }

// extern void NxDuinoSetState(unsigned char _state)
// {
//     state = _state ;
//     runOnce = true ;
// }

// static void nextState(unsigned char _state, unsigned char _interval)
// {
//     runOnce = true ;
//     exitFlag = false ;
//     if(_interval)
//     {
//         enabled = false;
//         NxDuinoT = _interval ;
//     }
//     state = _state ;
// }

// // STATE FUNCTIONS

// stateFunction(initNXduino) {
//     entryState
//     {
        
//     }
//     onState
//     {

//         exitFlag = true;
//     }
//     exitState
//     {

//         return true;
//     }
// }
// /*
//     currentX/Y is where we are now
//     newX/Y is where we are going to look for a new object
//     X1/Y1 or X2/Y2 must be pointing to currentX/Y
// */
// stateFunction(getButtons) 
// {
//     uint8_t startPin, stopPin ;
    
//     entryState
//     {
        
//     }
//     onState
//     {
            
        
// 		exitFlag = true;
//     }
//     exitState
//     {
//         return true;
//     }
// }

// stateFunction(findAdjacentObject) 
// {
//     uint8_t searchX, searchY ;
    
//     entryState
//     {
       
//     }
//     onState
//     {        
        
//         exitFlag = true;
//     }
//     exitState
//     {
//         if( type != NA) current = adjacent ;                  // copy adjacent obj to current obj
        
//         return true;
//     }
// }


// stateFunction(storeNode) 
// {
//     entryState
//     {
//         push( current.X, current.Y ) ;                           // push node coordinates on the node stack
//     }
//     onState
//     {
//         exitFlag = true;
//     }
//     exitState
//     {
//         return true;
//     }
// }

// stateFunction(loadNode) 
// {
//     uint8_t X, Y ;
    
//     entryState
//     {
//         pop( &X, &Y ) ;                                         // pop node coordinates from the node stack
//         loadNode( &current, X, Y);
//     }
//     onState
//     {
//         exitFlag = true;
//     }
//     exitState
//     {

//         return true;
//     }
// }

// stateFunction(fail) 
// {
//     entryState
//     {
        
//     }
//     onState
//     {
//         //if( blink( errorLed ) )
//         {
//             exitFlag = true;
//         }
//     }
//     exitState
//     {

//         return true;
//     }
// }

// stateFunction(succes) {
//     entryState
//     {
//     }
//     onState
//     {
//         //if( blink( succesLed ) )
//         {
//             exitFlag = true;
//         }
//     }
//     exitState
//     {

//         return true;
//     }
// }

// // stateFunction(storeLed) {
// //     entryState
// //     {
        
// //     }
// //     onState
// //     {
// //         push( led, inputPin) ;

// //         exitFlag = true;
// //     }
// //     exitState
// //     {

// //         return true;
// //     }
// // }

// // STATE MACHINE
// extern bool pathFinder(void) {
//     STATE_MACHINE_BEGIN
    
    
//     State(initNXduino) {
//         nextState(getButtons, 0); }

//     State(getButtons) {
//         nextState(findAdjacentObject, 0); }

//     State(findAdjacentObject) {
//         if( type == node )      nextState(storeNode, 0);
//         if( type == other )     nextState(findAdjacentObject, 0);
//         if( type == nothing ) {
//             if( nodesLeft )     nextState(loadNode, 0);
//             else                nextState(fail, 0); }
//         if( type == endButton)  nextState(succes, 0); }
//       //  if( type == led )       nextState(storeLed, 0); }

//     State(storeNode) {
//         nextState(findAdjacentObject, 0); }

//     State(loadNode) {
//         nextState(findAdjacentObject, 0); }

//     State(fail) {
//         nextState(getButtons, 0); }

//     State(succes) {
//         nextState(getButtons, 0); }

//     // State(storeLed) {
//     //     nextState(findAdjacentObject, 0); }

//     STATE_MACHINE_END
// }
