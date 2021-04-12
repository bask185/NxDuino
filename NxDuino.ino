//#include "NxDuino.h"
#include "roundRobinTasks.h"

void setup()
{
	roundRobinTasksInit() ;
	loadSDcard() ;			// checks if SD card is present and if it contains layout.csv. If so, filles EEPROM
	loadEEPROM() ;			// runs through EEPROM to initialize MCP23017 devices

	//NxDuinoInit() ;
	//Serial.begin(115200) ;
}

void loop()
{
	roundRobinTasks() ;
	//NxDuino() ;

}