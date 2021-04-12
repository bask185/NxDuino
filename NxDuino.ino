//#include "NxDuino.h"
#include "roundRobinTasks.h"

void setup()
{
	roundRobinTasksInit() ;
	//NxDuinoInit() ;
	//Serial.begin(115200) ;
}

void loop()
{
	roundRobinTasks() ;
	//NxDuino() ;

}