/******************************************************************
 *****                                                        *****
 *****  Name: Ethernet.c                                      *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 25/01/2019                                      *****
 *****  Auth: Vladislav Kravchenko                            *****
 *****        Alcala de Henares                               *****
 *****        University of Alcala                            *****
 *****        Spain                                           *****
 *****                                                        *****
 ******************************************************************/

// Para variar las direcciones IP (propia y gateway) y mascaras de red: tcpip.h
#include "LPC17xx.h"
#include "cmsis_os.h"
#include <string.h>
#include "stdlib.h"
#include "EMAC.h"
#include "rt_TypeDef.h"
#include "rt_MemBox.h"
#include "RTX_config.h"
#ifndef extern
#define extern
#endif
#include "easyweb.h"
#include "tcpip.h"

/*DECLARACION DE VARIABLES NECESARIOS*/
	uint16_t auxPort=0;
	uint8_t auxDir[4];
	uint8_t P_Open=0,A_Open=0;
	uint8_t i = 1;
/*MEMORY RESERVE*/
	P_TCB rt_tid2ptcb (osThreadId thread_id);
	P_ESYNC esync;
	_declare_box(eth_pool,1,1);
	unsigned int e_reserved = 0;
	_declare_box(epool,22,10); //Modify for requeriments

/*DECLARACION DE VARIABLES HEADER*/
//unsigned short activar_systick;
	unsigned int BytesToSend;
	unsigned char Status; // status byte 
	unsigned char *PWebSide;

	struct OS_TETH *first = NULL;
	struct OS_TETH *last = NULL;
	struct OS_TETH *run = NULL;


P_TETH __svc(4) open_ethernet(osThreadId ID);
P_TETH __SVC_4      				 (osThreadId ID){
	P_TCB     id;
	struct OS_TETH *dir = NULL;
	
	if(e_reserved == 0){
		activar_systick=1; //ACTIVAR LA CUENTA DE ISN Y TIMETICK
		TCPLowLevelInit(); //funcion de inicializacion TCP
		/*MEMORY RESERVE FOR STRUCT*/
		rt_init_box(epool,sizeof(epool),sizeof(struct OS_TETH)); //8
		rt_init_box(eth_pool,sizeof(eth_pool),sizeof(struct E_SYNC_OS));
		esync = rt_alloc_box(eth_pool);
		memset(esync,0,sizeof(struct E_SYNC_OS));
		e_reserved = 1;
	}
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if((esync->ID == id->task_id) || (esync->ID == 0)){
		esync->ID = id->task_id;
		dir = rt_alloc_box(epool);
		//dir = rt_alloc_mem(struct_dir,sizeof(struct OS_TETH));
		memset(dir,0,sizeof(struct OS_TETH));
		dir->ID = id->task_id; // se la asignamos 
	}
	return(dir);
}

/*FUNCION SVC ENVIAR DATOS VIA ETHERNET*/
void __svc(5) write_ethernet(char *data_tx, osThreadId ID);
void __SVC_5       					(char *data_tx, osThreadId ID){
	P_TCB id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(esync->ID == id->task_id){
		TCPReleaseRxBuffer(); //liberar buffer
			if (SocketStatus & SOCK_CONNECTED){ //Si ha conectado...
				if (SocketStatus & SOCK_TX_BUF_RELEASED){ //Y se ha liberado el buffer
					BytesToSend = strlen(data_tx); //Bytes a enviar
					PWebSide = (unsigned char *)data_tx;
					if (BytesToSend > MAX_TCP_TX_DATA_SIZE){ //Si superan el rango
						if (!(Status & HTTP_SEND_PAGE)){
							memcpy(TCP_TX_BUF,data_tx,MAX_TCP_TX_DATA_SIZE); //copia los MAX_TCP_TX_DATA_SIZE caracteres del objeto apuntado por data_tx al objeto apuntado por TCP_TX_BUF	
							memcpy(TCP_TX_BUF + sizeof(data_tx) - 1, PWebSide, MAX_TCP_TX_DATA_SIZE - sizeof(data_tx) + 1);
							BytesToSend -= MAX_TCP_TX_DATA_SIZE - sizeof(data_tx);
							PWebSide += MAX_TCP_TX_DATA_SIZE - sizeof(data_tx) + 1;
							TCPTxDataCount = MAX_TCP_TX_DATA_SIZE; //Se transmite la longitud maxima
							TCPTransmitTxBuffer();	
						}
						else{
							memcpy(TCP_TX_BUF,PWebSide,MAX_TCP_TX_DATA_SIZE);
							BytesToSend -= MAX_TCP_TX_DATA_SIZE;
							PWebSide += MAX_TCP_TX_DATA_SIZE;
						}
						TCPTxDataCount = MAX_TCP_TX_DATA_SIZE; 
						TCPTransmitTxBuffer();
					}
					else if(BytesToSend){
						memcpy(TCP_TX_BUF, data_tx, BytesToSend);//copia los BytesToSend caracteres del objeto apuntado por data_tx al objeto apuntado por TCP_TX_BUF	
						TCPTxDataCount = BytesToSend;                        
						TCPTransmitTxBuffer();                   
						TCPClose();					
						BytesToSend = 0;
					}
					Status |= HTTP_SEND_PAGE;
				}
			}
			else 
				Status &=~HTTP_SEND_PAGE;
	}
}
/*FUNCION SVC RECIBIR DATOS VIA ETHERNET*/
void __svc(6) read_ethernet(unsigned char *data_rx, osThreadId ID);
void __SVC_6               (unsigned char *data_rx, osThreadId ID){
	P_TCB     id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(esync->ID == id->task_id){
		if(SocketStatus & SOCK_CONNECTED){
			if(TCPRxDataCount){
				 memcpy(data_rx,TCP_RX_BUF,TCPRxDataCount);
			}	
			TCPReleaseRxBuffer();
		}
	}
}
void __svc(8) close_ethernet(osThreadId ID, struct OS_TETH *cls);
void __SVC_8                (osThreadId ID, struct OS_TETH *cls){
	P_TCB     id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(esync->ID == id->task_id){
		liberar(cls);
		if(first == NULL){
			activar_systick = 0;
			TCPFlags = 0;
			SocketStatus = 0;
			memset(esync,0,sizeof(struct E_SYNC_OS)); //Primero, limpiar el espacio
			rt_free_box(eth_pool,esync); //Segundo liberar la memoria
			e_reserved = 0;
			TCPStateMachine = CLOSED;
		}
	}
}
/**********************************************************
FUNCTION TO ADD ONE MORE STRUCT
PARAMETER: THE NEW STRUCT;
***********************************************************/
void add_element(struct OS_TETH *copy){
	struct OS_TETH *nuevo;
	nuevo = first;
	while(nuevo!=NULL){
		if(nuevo == copy){ //If there is a same structure
			i = 1;
			break;
		}
		else{
			nuevo = nuevo->next;
			if(nuevo == NULL){ //se ha llegado al final y no se ha encontrado structuras iguales
				i = 0;
				break;
			}
		}
	}
	if((i == 0) || (first == NULL)){ //se añade la nueva estructura	
		if(first == NULL){ //If first structure
			first = copy;
			last = copy;
		}	
		else {
			last->next = copy;
			last = copy;
		}
			i = 0;
		}
}
/**********************************************************
FUNCTION TO CHECK ALL STRUCTs TO FIND THE NECESSARY STRUCT
PARAMETER: ID OF THE THREAD THROUGH WHICH WE ARE GOING TO
						FIND THE NECESSARY STRUCT;
***********************************************************/
void check_struct (osThreadId ID,uint8_t num_con){
	struct OS_TETH *aux;
	P_TCB     ptcb;
	
	ptcb = rt_tid2ptcb(ID); //get task_id
	
	aux = first;
	while(aux != NULL){
		if(aux->ID == ptcb->task_id){
			if(aux->n_con == num_con){
				run = aux;
				break;
			}
			else
				aux = aux->next;
		}
		else 
			aux = aux->next;
	}
}
/**********************************************************
FUNCTION TO FREE MEMORY OF THE STRUCTs
PARAMETER: STRUCT TO ERASE
***********************************************************/
void liberar(struct OS_TETH *erase){
  struct OS_TETH *reco;
	reco = first;
    while (reco != NULL){
			if(erase->ID == reco->ID){
				if(erase->n_con == reco->n_con){
					erase = reco;
					reco = reco->next;
					memset(erase,0,sizeof(struct OS_TETH));
					rt_free_box(epool,erase);
				}
				else
					reco = reco->next;
			}
			else
				reco = reco->next;
    }
}
void __svc(7) connect_ethernet(struct OS_TETH *add, osThreadId ID,uint8_t num_con);
void __SVC_7                  (struct OS_TETH *add, osThreadId ID,uint8_t num_con){
	P_TCB     id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(esync->ID == id->task_id){
		add_element(add);
		check_struct(ID,num_con);
		/*ACTUANDO COMO CLIENTE*/
		if(run->S_C == 1){
			if(((int)run->IP_1>=0 && run->IP_1<=255) && ((int)run->IP_2>=0 && run->IP_2<=255) && ((int)run->IP_3>=0 && run->IP_3<=255) && ((int)run->IP_4>=0 && run->IP_4<=255)){ // se ha introducido una IP valida
				if(auxDir[0] != run->IP_1 || auxDir[1] != run->IP_2 || auxDir[2] != run->IP_3 || auxDir[3] != run->IP_4){ //Ip diferente a la IP anterior
					*(unsigned char *)RemoteIP = run->IP_1;            
					*((unsigned char *)RemoteIP + 1) = run->IP_2;          
					*((unsigned char *)RemoteIP + 2) = run->IP_3;         
					*((unsigned char *)RemoteIP + 3) = run->IP_4;
					auxDir[0]=run->IP_1;
					auxDir[1]=run->IP_2;
					auxDir[2]=run->IP_3;
					auxDir[3]=run->IP_4;
					TCPLocalPort=2025;
				}
				TCPRemotePort = run->P_EXT;
			}
		if (!(SocketStatus & SOCK_ACTIVE) || (P_Open==1) || (TCPRemotePort != (uint16_t)run->P_EXT)){
			//TCPStateMachine = CLOSED;
			TCPActiveOpen();//establcer conexion con la direccion indicada(cliente)
			P_Open=0;
			A_Open=1;
		}
		}
	/*ACTUANDO COMO SERVIDOR*/
		else { 
			if(TCPLocalPort!=(int)run->P_INT){
				TCPLocalPort = run->P_INT;//escuchar en el puerto indicado
				auxPort=run->P_INT;
			}
			if (!(SocketStatus & SOCK_ACTIVE) || (A_Open==1)){
				//TCPStateMachine = CLOSED;
				TCPPassiveOpen();//escuchar el puerto local, esperando conexion (servidor)
				A_Open=0;
				P_Open=1;
			}
		}
	}
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


