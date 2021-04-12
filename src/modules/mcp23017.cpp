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
