#include "variables.h"

railItems IO[nRailItem] ;
trackSegments track[ nTrackSegment ] ;

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