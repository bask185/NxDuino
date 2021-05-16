#include "variables.h"

Mcp23017 mcp[]
{
	Mcp23017(),
	Mcp23017(),
	Mcp23017(),
	Mcp23017(),
	Mcp23017(),
	Mcp23017(),
	Mcp23017(),
	Mcp23017() 
} ;

uint8_t nMcp = 8 ;

uint16_t outputs[ 8 ] ;

railItems IO ;
trackSegments track ;

