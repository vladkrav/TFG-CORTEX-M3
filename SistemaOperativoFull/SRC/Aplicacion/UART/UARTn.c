/*Hoja de codigo de las UART*/
#include "cmsis_os.h"
#include <LPC17xx.h>
#include <string.h>
#include "rt_MemBox.h"
#include "rt_TypeDef.h"
#include "RTX_config.h"
#include "uartn.h"

	char bufferUART[512];	// Buffer de recepción
	char *ptr_rx_0, *ptr_rx_1, *ptr_rx_2, *ptr_rx_3;	// puntero de recepción
	char rx_completa_0, rx_completa_1, rx_completa_2, rx_completa_3; // Flag de recepción de cadena completa que se activa al recibir CR(0x0D)
	char *ptr_tx_0, *ptr_tx_1, *ptr_tx_2, *ptr_tx_3;			// puntero de transmisión
	char tx_completa_0,tx_completa_1,tx_completa_2,tx_completa_3; // Flag de transmisión de cadena completa
	char enviado[30];
	char mensaje[30];
	uint8_t solo_rx = 0;
	uint8_t solo_tx = 0;
	/*MEMORY RESERVE*/
	P_USYNC usync;
	_declare_box(uart_pool,1,1); //Modify for requeriments
	unsigned int u_reserved = 0; //memory reserved for file struct
	P_TCB rt_tid2ptcb (osThreadId thread_id);

static int uartn_set_baudrate(uint8_t UARTn, unsigned int baudrate) {
    int errorStatus = -1; //< Fallo de calculo
	
    unsigned int PCLK = SystemCoreClock/4;
    unsigned int calcBaudrate = 0;
    unsigned int temp = 0;

    unsigned int mulFracDiv, dividerAddFracDiv;
    unsigned int divider = 0;
    unsigned int mulFracDivOptimal = 1;
    unsigned int dividerAddOptimal = 0;
    unsigned int dividerOptimal = 0;

    unsigned int relativeError = 0;
    unsigned int relativeOptimalError = 100000;

    PCLK = PCLK >> 4; /* dividido por 16 */

    /*
     *  La formula es:
     * BaudRate = PCLK * (mulFracDiv/(mulFracDiv+dividerAddFracDiv) / (16 * DLL)
     *
     * The value of mulFracDiv and dividerAddFracDiv should comply to the following expressions:
     * 0 < mulFracDiv <= 15, 0 <= dividerAddFracDiv <= 15
     */
    for (mulFracDiv = 1; mulFracDiv <= 15; mulFracDiv++) {
        for (dividerAddFracDiv = 0; dividerAddFracDiv <= 15; dividerAddFracDiv++) {
            temp = (mulFracDiv * PCLK) / (mulFracDiv + dividerAddFracDiv);

            divider = temp / baudrate;
            if ((temp % baudrate) > (baudrate / 2))
                divider++;

            if (divider > 2 && divider < 65536) {
                calcBaudrate = temp / divider;

                if (calcBaudrate <= baudrate) {
                    relativeError = baudrate - calcBaudrate;
                } else {
                    relativeError = calcBaudrate - baudrate;
                }

                if (relativeError < relativeOptimalError) {
                    mulFracDivOptimal = mulFracDiv;
                    dividerAddOptimal = dividerAddFracDiv;
                    dividerOptimal = divider;
                    relativeOptimalError = relativeError;
                    if (relativeError == 0)
                        break;
                }
            }
        }

        if (relativeError == 0)
            break;
    }

    if (relativeOptimalError < ((baudrate * UART_ACCEPTED_BAUDRATE_ERROR) / 100)) {
		
        switch(UARTn){
			case 0 :
				LPC_UART0->LCR |= DLAB_ENABLE; 	//Habilita el acceso para configuracion
				LPC_UART0->DLM = (unsigned char) ((dividerOptimal >> 8) & 0xFF);
				LPC_UART0->DLL = (unsigned char) dividerOptimal;
				LPC_UART0->LCR &= ~DLAB_ENABLE;	//Deshabilita el acceso, se ha terminado la configuracion 
				LPC_UART0->FDR = ((mulFracDivOptimal << 4) & 0xF0) | (dividerAddOptimal & 0x0F);
				errorStatus = 0; //Sin error
				break; 
			case 1 :
				LPC_UART1->LCR |= DLAB_ENABLE;  //Habilita el acceso para configuracion
				LPC_UART1->DLM = (unsigned char) ((dividerOptimal >> 8) & 0xFF);
				LPC_UART1->DLL = (unsigned char) dividerOptimal;
				LPC_UART1->LCR &= ~DLAB_ENABLE;//Deshabilita el acceso, se ha terminado la configuracion
				LPC_UART1->FDR = ((mulFracDivOptimal << 4) & 0xF0) | (dividerAddOptimal & 0x0F);
				errorStatus = 0; //Sin error
				break;
			case 2 :
				LPC_UART2->LCR |= DLAB_ENABLE;//Habilita el acceso para configuracion
				LPC_UART2->DLM = (unsigned char) ((dividerOptimal >> 8) & 0xFF);
				LPC_UART2->DLL = (unsigned char) dividerOptimal;
				LPC_UART2->LCR &= ~DLAB_ENABLE;//Deshabilita el acceso, se ha terminado la configuracion
				LPC_UART2->FDR = ((mulFracDivOptimal << 4) & 0xF0) | (dividerAddOptimal & 0x0F);
				errorStatus = 0; //Sin error
				break; 
			case 3 :
				LPC_UART3->LCR |= DLAB_ENABLE; 	//Habilita el acceso para configuracion 
				LPC_UART3->DLM = (unsigned char) ((dividerOptimal >> 8) & 0xFF);
				LPC_UART3->DLL = (unsigned char) dividerOptimal;
				LPC_UART3->LCR &= ~DLAB_ENABLE;	//Deshabilita el acceso, se ha terminado la configuracion
				LPC_UART3->FDR = ((mulFracDivOptimal << 4) & 0xF0) | (dividerAddOptimal & 0x0F);
				errorStatus = 0; //Sin error
				break; 
		}		
    }

    return errorStatus;
}
void __svc(1) open_uart(uint8_t UARTn, uint32_t baudrate, osThreadId ID);
void __SVC_1 			     (uint8_t UARTn, uint32_t baudrate, osThreadId ID){
	/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
	/*MEMORY RESERVE FOR STRUCT*/
	if(u_reserved == 0){
		rt_init_box(uart_pool,sizeof(uart_pool),sizeof(struct U_SYNC_OS));
		usync = rt_alloc_box(uart_pool);
		memset(usync,0,sizeof(struct U_SYNC_OS));
		u_reserved = 1;
	}
	/*EXTRACTING ID OF THE THREAD*/
	id = rt_tid2ptcb(ID);
	/*RUNNING THE CONFIGURATION*/
	if((usync->ID == id->task_id) || (usync->ID == 0)){
		usync->ID = id->task_id;
		/*Punteros*/
		ptr_rx_0=bufferUART;
		ptr_rx_1=bufferUART;
		ptr_rx_2=bufferUART;
		ptr_rx_3=bufferUART;
		switch(UARTn){
			case UART0 :
				LPC_PINCON->PINSEL0|=(1<<4)|(1<<6); //Configuracion pines RXD0 y TXD0  
				LPC_UART0->LCR &= ~STOP_1_BIT & ~PARITY_NONE; // Set 8N1 mode (8 bits/dato, sin pariad, y 1 bit de stop)
				LPC_UART0->LCR |= CHAR_8_BIT;

				uartn_set_baudrate(UARTn, baudrate);// Se calcula el baudrate
			
				LPC_UART0->IER = THRE_IRQ_ENABLE|RBR_IRQ_ENABLE;// Se habilita las interrupciones TX y RX  
				NVIC_EnableIRQ(UART0_IRQn);// Enable the UART interrupt (for Cortex-CM3 NVIC)
				break;
			
			case UART1 :
				LPC_PINCON->PINSEL0|= (1<<30); //Configuracion pin y TXD1
				LPC_PINCON->PINSEL1|= (1<<0); //Configuracion pin RXD1
				LPC_UART1->LCR &= ~STOP_1_BIT & ~PARITY_NONE;
				LPC_UART1->LCR |= CHAR_8_BIT;
			
				uartn_set_baudrate(UARTn, baudrate);// Se calcula el baudrate
			
				LPC_UART1->IER = THRE_IRQ_ENABLE | RBR_IRQ_ENABLE;
				NVIC_EnableIRQ(UART1_IRQn);
				break;
			case UART2 :
				LPC_PINCON->PINSEL0|= (1<<20)|(1<<22); //Configuracion pines RXD2 y TXD2
				LPC_SC->PCONP|=(1<<24); //Se  habilita la UART2
				LPC_UART2->LCR &= ~STOP_1_BIT & ~PARITY_NONE;
				LPC_UART2->LCR |= CHAR_8_BIT;
		
				uartn_set_baudrate(UARTn, baudrate);// Se calcula el baudrate
			
				LPC_UART2->IER = THRE_IRQ_ENABLE|RBR_IRQ_ENABLE;
				NVIC_EnableIRQ(UART2_IRQn);
				break;
			case UART3 :
				LPC_PINCON->PINSEL9|= (3<<24)|(3<<26); //Configuracion pines RXD3 y TXD3 P4.28 - P4.29
				LPC_SC->PCONP|=(1<<25); //Se habilita la UART3
				LPC_UART3->LCR &= ~STOP_1_BIT & ~PARITY_NONE;
				LPC_UART3->LCR |= CHAR_8_BIT;
		
				uartn_set_baudrate(UARTn, baudrate);// Se calcula el baudrate
			
				LPC_UART3->IER = THRE_IRQ_ENABLE | RBR_IRQ_ENABLE;
				NVIC_EnableIRQ(UART3_IRQn);	
				break;
		}
	}	
}
void __svc(2) write_uart(uint8_t UARTn, char *datos, osThreadId ID); //enviar datos
void __SVC_2 			      (uint8_t UARTn, char *datos, osThreadId ID){
	/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
		if(solo_tx == 0){
		ptr_tx_0=datos;
		ptr_tx_1=datos;
		ptr_tx_2=datos;
		ptr_tx_3=datos;
		solo_tx = 1;
	}
	id = rt_tid2ptcb(ID);
	if(usync->ID == id->task_id){
		switch(UARTn){
			case UART0 :
				while(tx_completa_0==0){
				ptr_tx_0=datos;
				tx_completa_0=1;
				LPC_UART0->THR=*ptr_tx_0++; //se introduce el primer dato para activar el flag de interrupcion;
				}
				break;
			case UART1 :
				while(tx_completa_1==0){
				ptr_tx_1=datos;
				tx_completa_1=1;
				LPC_UART1->THR=*ptr_tx_1++; //se introduce el primer dato para activar el flag de interrupcion;
				}
				break;
			case UART2 :
				while(tx_completa_2==0){
				ptr_tx_2=datos;
				tx_completa_2=1;
				LPC_UART2->THR=*ptr_tx_2++; //se introduce el primer dato para activar el flag de interrupcion;
				}
				break;
			case UART3 :
				while(tx_completa_3==0){
				ptr_tx_3=datos;
				tx_completa_3=1;
				LPC_UART3->THR=*ptr_tx_3++; //se introduce el primer dato para activar el flag de interrupcion;
				}
				break;
		}
	}
}
void __svc(3) read_uart(uint8_t UARTn, char *datos_rx, osThreadId ID); //enviar datos
void __SVC_3 			     (uint8_t UARTn, char *datos_rx, osThreadId ID){
	P_TCB id;
	if(solo_rx == 0){
		ptr_rx_0=datos_rx;
		ptr_rx_1=datos_rx;
		ptr_rx_2=datos_rx;
		ptr_rx_3=datos_rx;
		solo_rx = 1;
	}
	id = rt_tid2ptcb(ID);
	if(usync->ID == id->task_id){
		switch(UARTn){
			case UART0 :
				//while(rx_completa_0==0);
				if(rx_completa_0 == 0)
					break;
				memcpy(datos_rx,ptr_rx_0,strlen(ptr_rx_0));
				rx_completa_0=0;
				break;
			case UART1 :
				if(rx_completa_1 == 0)
					break;
				memcpy(datos_rx,ptr_rx_1,strlen(ptr_rx_1));
				rx_completa_1=0;
				break;
			case UART2 :
				if(rx_completa_2 == 0)
					break;
				memcpy(datos_rx,ptr_rx_2,strlen(ptr_rx_2));
				rx_completa_2=0;
				break;
			case UART3 :
				if(rx_completa_3 == 0)
					break;
				memcpy(datos_rx,ptr_rx_3,strlen(ptr_rx_3));
				rx_completa_3=0;
				break;
		}
	}
}
void __svc(19) close_uart(osThreadId ID);
void __SVC_19				     (osThreadId ID){
	P_TCB id;
	id = rt_tid2ptcb(ID);
	if(usync->ID == id->task_id){
		memset(usync,0,sizeof(struct U_SYNC_OS)); //Primero, limpiar el espacio
		rt_free_box(uart_pool,usync); //Segundo liberar la memoria
		u_reserved = 0;
		solo_rx = 0;
		solo_tx = 0;
	}
}
void UART0_IRQHandler(void) {

    switch(LPC_UART0->IIR&0x0E) {
	
	case 0x04:								 /* RBR, Receiver Buffer Ready */
		*ptr_rx_0=LPC_UART0->RBR; 	   			 /* lee el dato recibido y lo almacena */
	    if (*ptr_rx_0++ == 13){			// Caracter return --> Cadena completa
					*ptr_rx_0 = 10;
					*ptr_rx_0++ = 0;		/* Añadimos el caracter null para tratar los datos recibidos como una cadena*/ 
					rx_completa_0 = 1;  /* rx completa */
					ptr_rx_0=bufferUART;	/* puntero al inicio del bufferUART para nueva recepción */
	    }	
		break;
			
   case 0x02:								/* THRE, Transmit Holding Register empty */
		if (*ptr_tx_0!=0)
			LPC_UART0->THR=*ptr_tx_0++;		/* carga un nuevo dato para ser transmitido */
		else tx_completa_0=0; 				//se pone a 1 diciendo que ha completado la transferencia
		break;
    }
}

void UART1_IRQHandler(void) {
	
    switch(LPC_UART1->IIR&0x0E) {
	
	case 0x04:								 /* RBR, Receiver Buffer Ready */
		*ptr_rx_1=LPC_UART1->RBR; 	   			 /* lee el dato recibido y lo almacena */
	    if (*ptr_rx_1++ == 13){				// Caracter return --> Cadena completa
				*ptr_rx_1 = 10;
	    	*ptr_rx_1=0;		/* Añadimos el caracter null para tratar los datos recibidos como una cadena*/ 
				rx_completa_1 = 1;/* rx completa */
				ptr_rx_1=bufferUART;	/* puntero al inicio del bufferUART para nueva recepción */
		}	
		break;
   case 0x02:								/* THRE, Transmit Holding Register empty */
		if (*ptr_tx_1!=0) 
			LPC_UART1->THR=*ptr_tx_1++;	/* carga un nuevo dato para ser transmitido */
		else tx_completa_1=0;
		break;

    }
}

void UART2_IRQHandler(void) {
	
    switch(LPC_UART2->IIR&0x0E) {
	
	case 0x04:								 /* RBR, Receiver Buffer Ready */
	*ptr_rx_2=LPC_UART2->RBR; 	   			 /* lee el dato recibido y lo almacena */
	    if (*ptr_rx_2++ == 13){			// Caracter return --> Cadena completa
				*ptr_rx_2 = 10;
	    	*ptr_rx_2=0;		/* Añadimos el caracter null para tratar los datos recibidos como una cadena*/ 
				rx_completa_2 = 1;/* rx completa */
				ptr_rx_2=bufferUART;	/* puntero al inicio del bufferUART para nueva recepción */
	    }	
		break;
	
    
   case 0x02:								/* THRE, Transmit Holding Register empty */
		if (*ptr_tx_2!=0) 
			LPC_UART2->THR=*ptr_tx_2++;	/* carga un nuevo dato para ser transmitido */
		else tx_completa_2=0;
		break;

    }
}

void UART3_IRQHandler(void) {
	
    switch(LPC_UART3->IIR&0x0E) {
		case 0x04:								 /* RBR, Receiver Buffer Ready */
			*ptr_rx_3=LPC_UART3->RBR; 	   			 /* lee el dato recibido y lo almacena */
			if (*ptr_rx_3++ == 13){ 				// Caracter return --> Cadena completa
					*ptr_rx_3 = 10;
	    		*ptr_rx_3 = 0;		/* Añadimos el caracter null para tratar los datos recibidos como una cadena*/ 
					rx_completa_3 = 1;/* rx completa */
				ptr_rx_3=bufferUART;	/* puntero al inicio del bufferUART para nueva recepción */
	    	}	
			break;
		case 0x02:								/* THRE, Transmit Holding Register empty */
			if (*ptr_tx_3!=0) 
				LPC_UART3->THR=*ptr_tx_3++;	/* carga un nuevo dato para ser transmitido */
			else tx_completa_3=0;
			break;

    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
