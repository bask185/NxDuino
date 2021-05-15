#include "mcp23017.h"
#include <Wire.h>

//enum mcp23017 {
//    IODIRA = 0x00,
//    IODIRB = 0x01,
//    pullUpRegA = 0x0c,
//    pullUpRegB = 0x0d,
//    portA = 0x12,
//    portB = 0x13,
//    input = 0xff,
//    output = 0x00,
//    pullUp = 0xff};

/* calculates which pin, which port and which mcp23017 slave is to be set or cleared */
void mcpWrite(unsigned char pin, unsigned char state) 
{
    unsigned char address = 0x20, port, IO;
    address += (pin / 16);                // select address

    if(pin % 16 < 8)    port = portA;    // select port
    else                port = portB;

    pin %= 8 ;

    Wire.beginTransmission(address);
    Wire.write(port);
    Wire.endTransmission();
    Wire.requestFrom(address, 1);
    IO = Wire.read();                    // fetch current IO status

    Wire.beginTransmission(address);
    Wire.write(port);
    if(state)    IO |=  (1 << pin);        // OR 'IO' with pin
    else         IO &= ~(1 << pin);        // or AND the inverse of pin
    Wire.write(IO);                        // transmit the updated IO
    Wire.endTransmission();
}

Mcp23017::Mcp23017() {;}

uint8_t Mcp23017::init(byte _address, unsigned int ioDir) {
    address = _address;
    Wire.beginTransmission(address);
    Wire.write(IODIRA);
    Wire.write(ioDir>>8);
    Wire.write(ioDir);
    uint8_t error = Wire.endTransmission();

    if( error ) return 1;                // slave is not present or defective

    Wire.beginTransmission(address);
    Wire.write(pullUpRegA);
    Wire.write(ioDir>>8);                   // pullup setting must be the same for iodir setting
    Wire.write(ioDir);
    Wire.endTransmission();
    
    Wire.beginTransmission(address);        // initialize all outputs to 0
    Wire.write(portA);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
    
    return 0;
}

void Mcp23017::setIOdir(byte iodirA, byte iodirB) {
    Wire.beginTransmission(address);
    Wire.write(IODIRA);
    Wire.write(iodirA);
    Wire.write(iodirB);
    Wire.endTransmission(); }

void Mcp23017::setPullUp(byte pullUpA, byte pullUpB) {
    Wire.beginTransmission(address);
    Wire.write(pullUpRegA);
    Wire.write(pullUpA);
    Wire.write(pullUpB);
    Wire.endTransmission(); }

void Mcp23017::setPortA(byte value) {
    Wire.beginTransmission(address);
    Wire.write(portA);
    Wire.write(value); 
    Wire.endTransmission();}

void Mcp23017::setPortB(byte value) {
    Wire.beginTransmission(address);
    Wire.write(portB);
    Wire.write(value); 
    Wire.endTransmission();}

void Mcp23017::setPort(byte port, byte value) {
    Wire.beginTransmission(address);
    Wire.write(port);
    Wire.write(value); 
    Wire.endTransmission();}

byte Mcp23017::getRegister(byte _register) {
    Wire.beginTransmission(address);
    Wire.write(_register);
    Wire.endTransmission();
    Wire.requestFrom(address, 1);
    return Wire.read(); }
