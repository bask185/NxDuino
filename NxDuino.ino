#include "NxDuino.h"

void setup()
{
	roundRobinTasksInit() ;
	NxDuinoInit() ;
}

void loop()
{
	roundRobinTasks() ;
	NxDuino() ;

}