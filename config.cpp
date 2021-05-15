#include "config.h"
char sbuf[70] ;

void Debug(String txt) 
{
    #ifndef XPRESSNET
    Serial.println(txt);
    #endif
}
void Debug(char* txt) 
{
    #ifndef XPRESSNET
    Serial.println(txt);
    #endif
}

void Debug(int number) 
{
    #ifndef XPRESSNET
    Serial.println(number);
    #endif
}
