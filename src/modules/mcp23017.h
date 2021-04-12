#ifndef Mcp23017_h
#define Mcp23017_h

#include <Arduino.h>

#define getInput(x) getRegister(x)
#define mcpBaseAddress 0x20

enum mcp23017 {
    IODIRA = 0x00,
    IODIRB = 0x01,
    pullUpRegA = 0x0c,
    pullUpRegB = 0x0d,
    portA = 0x12,
    portB = 0x13 };

class Mcp23017 {
    public:
    Mcp23017();
    uint8_t init(byte, unsigned int);
    void setIOdir(byte, byte);
    void setPullUp(byte, byte);
    void setPortA(byte);
    void setPortB(byte);
    void setPort(byte, byte);
    byte getRegister(byte);
    
    private:
    byte address; };

#endif


