/*********************/
/* INCLUDEs & DEFINEs*/
/*********************/
#include "cmsis_os.h"
#include "LPC17xx.h"
#include "stdlib.h"
#include "TouchPanel_OS.h"

/***********/
/*VARIABLES*/
/***********/
int k;
/*creacion de hilos */
//void threadX (void const *argument);
//void threadY (void const *argument);
unsigned char mensaje_eth[256];
osThreadId main_id;
osThreadId threadX_id;
osThreadId threadY_id;
//osThreadDef(threadY, osPriorityNormal, 1, 0);
//osThreadDef(threadX, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void){
	main_id = osThreadGetId();
	open_touch(main_id);
	while(1){
	write_touch(main_id);
	}
}
