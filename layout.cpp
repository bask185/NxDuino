// #include <SD.h>

// layout = SD.open("layout.txt") ;

// while( layout.available() ) {
    
//     byte buffer[someSize] ;
//     layout.readBytes( buffer someSize ) ;
    
    
    
    
    
// typedef struct {
//     uint8_t type ;
//     uint8_t dir ;
//     uint8_t x1 ;
//     uint8_t y1 ; // point has 3 connections
//     uint8_t x2 ;
//     uint8_t y2 ;
//     uint8_t x3 ;
//     uint8_t y3 ;
// } Object ;


    
// getAdjacentObject( &x1, &y1, &x2, &y2 )


/*
first check if there is an object present on the search location
than check if that obect is also connected to the start object
we must also check to which side we are connected.


//  curve object
// dir   1   2   3   4   5   6   7   8
*/
// const int offsets[4][8][8] = 
// { 
//     {//dir 1   2   3   4   5   6   7   8    
//         {  0,  1,  1,  1, -1, -1, -1,  1 } ,        // curve
//         {  1,  1,  0, -1,  0, -1,  0, -1 } , 
//         { -1,  0,  1,  1,  1,  0, -1, -1 } , 
//         { -1, -1, -1,  0,  1,  1,  1,  0 } , 
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA 
//         {  0,  0,  0,  0,  0,  0,  0,  0 }          // NA
//     } ,
//     {
//         {  1,  1,  0, -1, -1, -1,  0,  1 } ,        // line (and most other objects)
//         {  0,  1,  1,  1,  0, -1, -1, -1 } ,
//         { -1, -1,  0,  1,  1,  1,  0, -1 } ,
//         {  0, -1, -1, -1,  0,  1,  1,  1 } ,
//         {  0,  0,  0,  0,  0,  0,  0,  0 } ,        // NA
//         {  0,  0,  0,  0,  0,  0,  0,  0 }          // NA
//     } ,
//     {
//         {  1,  1,  0, -1, -1, -1,  0,  1 } ,        // switch left
//         {  0,  1,  1,  1,  0, -1, -1, -1 } ,
//         { -1, -1,  0,  1,  1,  1,  0, -1 } ,
//         {  0, -1, -1, -1,  0,  1,  1,  1 } ,
//         { -1, -1, -1,  0,  1,  1,  1,  0 } ,
//         {  1,  0, -1, -1, -1,  0,  1,  1 }
//     } ,
//     {
//         {  1,  1,  0, -1, -1, -1,  0,  1 } ,        // switch right
//         {  0,  1,  1,  1,  0, -1, -1, -1 } ,
//         { -1, -1,  0,  1,  1,  1,  0, -1 } ,
//         {  0, -1, -1, -1,  0,  1,  1,  1 } ,
//         { -1,  0,  1,  1,  1,  0, -1, -1 } ,
//         {  1, -1, -1,  0,  1, -1,  1,  0 } 
//     }
// }

/* EEPROM STORAGE











/* curve
// X1    0,  1,  1,  1, -1, -1, -1,  1
// Y1    1,  1,  0, -1,  0, -1,  0, -1
// X2   -1,  0,  1,  1,  1,  0, -1, -1
// Y2   -1, -1, -1,  0,  1,  1,  1,  0

//  line object         // LATTER 4 MIGHT BE REDUNDANT  may also be used for lights and buttons
// dir   1   2   3   4  |5   6   7   8
// X1    1,  1,  0, -1, -1, -1,  0,  1
// Y1    0,  1,  1,  1, |0, -1, -1, -1
// X2   -1, -1,  0,  1, |1,  1,  0, -1
// Y2    0, -1, -1, -1, |0,  1,  1,  1

//  switch left
// dir   1   2   3   4   5   6   7   8
// X1    1,  1,  0, -1, -1, -1,  0,  1
// Y1    0,  1,  1,  1,  0, -1, -1, -1
// X2   -1, -1,  0,  1,  1,  1,  0, -1
// Y2    0, -1, -1, -1,  0,  1,  1,  1
// X3   -1, -1, -1,  0,  1,  1,  1,  0
// Y3    1,  0, -1, -1, -1,  0,  1,  1

//  switch right
// dir   1   2   3   4   5   6   7   8
// X1    1,  1,  0, -1, -1, -1,  0,  1
// Y1    0,  1,  1,  1,  0, -1, -1, -1
// X2   -1, -1,  0,  1,  1,  1,  0, -1
// Y2    0, -1, -1, -1,  0,  1,  1,  1
// X3   -1,  0,  1,  1,  1,  0, -1, -1
// Y3    1, -1, -1,  0,  1, -1,  1,  0

PSEUO CODE
while( !endFound ) {

    for (currentObj in objects) {        // this is not python need syntax fixing
    
        if( currentObj -> x == adjacentObj -> x &&  currentObj -> y == adjacentObj -> y) {
            currentObj = adjacentObj ; need syntax fixing with pointers and adresses
            
            if( type == button && ID == endButton )    // if the end button is found
            {
                goto buttonFound ;
            
            if( no connections
            */