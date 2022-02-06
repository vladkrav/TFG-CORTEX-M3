/****************************************************************/
/****************************UART.h******************************/
/****************************************************************/
#include "lpc17xx.h"
#include "cmsis_os.h"

#ifndef UART_H_
#define UART_H_

// Accepted Error baud rate value (in percent unit)
#define UART_ACCEPTED_BAUDRATE_ERROR    3

#define CHAR_8_BIT                      (3 << 0)
#define STOP_1_BIT                      (0 << 2)
#define PARITY_NONE                     (0 << 3)
#define DLAB_ENABLE                     (1 << 7)
#define FIFO_ENABLE                     (1 << 0)
#define RBR_IRQ_ENABLE                  (1 << 0)
#define THRE_IRQ_ENABLE                 (1 << 1)
#define UART_LSR_THRE   								(1 << 5)
#define RDA_INTERRUPT                   (2 << 1)
#define CTI_INTERRUPT                   (6 << 1)

extern char bufferUART[512];	// Buffer de recepción
extern char *ptr_rx_0, *ptr_rx_1, *ptr_rx_2, *ptr_rx_3;	// puntero de recepcion
extern char rx_completa_0,rx_completa_1,rx_completa_2,rx_completa_3;// Flag de recepcion de cadena completa que se activa al recibir CR(0x0D)
extern char *ptr_tx_0, *ptr_tx_1, *ptr_tx_2, *ptr_tx_3;			// puntero de transmision
extern char tx_completa_0,tx_completa_1,tx_completa_2,tx_completa_3;		// Flag de transmision de 
extern char enviado[30];

extern void __svc(1) open_uart(uint8_t UARTn, uint32_t baudrate, osThreadId ID);
extern void __svc(2) write_uart(uint8_t UARTn,char *datos, osThreadId ID);
extern void __svc(3) read_uart(uint8_t UARTn,char *datos_rx, osThreadId ID);
extern void __svc(19) close_uart(osThreadId ID);
extern char mensaje[30];

/*USE THIS FOR CHOOSE THE UART*/
#define UART0		0
#define UART1		1
#define UART2		2
#define UART3   3

/*SYNCHRONIZATION BETWEEN THREADs*/
typedef struct U_SYNC_OS{
	unsigned char ID;
}*P_USYNC;


#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
