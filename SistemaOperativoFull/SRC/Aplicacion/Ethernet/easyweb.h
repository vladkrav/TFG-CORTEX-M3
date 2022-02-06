//#include "LPC17xx.h"
#include "cmsis_os.h"

#ifndef __EASYWEB_H
#define __EASYWEB_H

typedef struct OS_TETH {
	unsigned char ID;       //unsigned char id_task;
	uint8_t IP_1;
	uint8_t IP_2;
	uint8_t IP_3;
	uint8_t IP_4;
	uint8_t S_C;
	uint16_t P_EXT;
	uint16_t P_INT;
	uint8_t n_con;
	struct OS_TETH *next;
}*P_TETH;

typedef struct E_SYNC_OS{
	unsigned char ID;
}*P_ESYNC;

//extern unsigned char Status;                        // status byte 
//extern unsigned char *PWebSide;
#define HTTP_SEND_PAGE               0x01        // help flag

P_TETH __svc(4) open_ethernet(osThreadId ID);
extern void __svc(5) write_ethernet(char *data_tx, osThreadId ID);
extern void __svc(6) read_ethernet(unsigned char *data_rx, osThreadId ID);
extern void __svc(7) connect_ethernet(struct OS_TETH *add, osThreadId ID,uint8_t num_con);
extern void __svc(8) close_ethernet(osThreadId ID, struct OS_TETH *cls);
//extern unsigned int BytesToSend; // bytes left to send
extern unsigned short activar_systick;

//extern void *rt_alloc_box  (void *box_mem);
/*FUNCTIONS ETHERNET.c*/
void add_element(struct OS_TETH *copy);
void check_struct (osThreadId ID,uint8_t num_con);
void liberar(struct OS_TETH *erase);
/*Define to use in Ethernet*/
#define SERVER 		0
#define CLIENT  	1



#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

