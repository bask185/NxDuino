#include "NxDuino.h"
#include "roundRobinTasks.h"

void setup()
{
	roundRobinTasksInit() ;
	//NxDuinoInit() ;
}

void loop()
{
	roundRobinTasks() ;
	//NxDuino() ;

}