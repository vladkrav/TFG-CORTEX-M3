/*********************/
/* INCLUDEs & DEFINEs*/
/*********************/
#include "cmsis_os.h"
#include "LPC17xx.h"
//#define extern
#include "easyweb.h"
#include "EMAC.h"
#include "tcpip.h"

/***********/
/*VARIABLES*/
/***********/
int k;
/*creacion de un hilo */
void threadX (void const *argument);
void threadY (void const *argument);
unsigned char mensaje_eth[256];
osThreadId main_id;
osThreadId threadX_id;
osThreadId threadY_id;
osThreadDef(threadY, osPriorityNormal, 1, 0);
//osThreadDef(threadX, osPriorityNormal, 1, 0);
//void threadY(){
//	P_TETH cf;
//	cf = open(threadY_id);
//	/*INICIALIZACION ESTRUCTURA*/
//	cf->IP_1 = 192;
//	cf->IP_2 = 168;
//	cf->IP_3 = 1;
//	cf->IP_4 = 135;
//	cf->S_C = CLIENT;
//	cf->P_EXT = 6000;
//	cf->P_INT = 80;
//	cf->n_con = 1;
//	
//	connect(cf, threadY_id,0);
//	write("ccccccccccccccc\n",threadY_id);
//}

/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void){
		int mensaje = 0;
		P_TETH config;
//P_TETH cf;
	main_id = osThreadGetId();
	config = open_ethernet(main_id);
	/*INICIALIZACION ESTRUCTURA*/
	config->IP_1 = 192;
	config->IP_2 = 168;
	config->IP_3 = 1;
	config->IP_4 = 135;
	config->S_C = CLIENT;
	config->P_EXT = 6000;
	config->P_INT = 80;
	config->n_con = 0;
	
//threadX_id = osThreadCreate(osThread(threadX), NULL);
threadY_id = osThreadCreate(osThread(threadY), NULL);
	while(1){
			connect_ethernet(config,main_id,0);
			read_ethernet(mensaje_eth, main_id);
			if(mensaje == 0){
				write_ethernet("Conexion Placa 1\n",main_id);
				mensaje++;
				}
			if(mensaje == 1){
				write_ethernet("Mensaje 2",main_id);
				mensaje++;
				}
			if(mensaje == 2){
				write_ethernet("Mensaje 3",main_id);
				mensaje++;
				}
				mensaje = 0;
			}
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

