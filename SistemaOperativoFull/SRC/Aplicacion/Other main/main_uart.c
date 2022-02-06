#include "cmsis_os.h"
#include "LPC17xx.h"
#include "uartn.h"
#include "string.h"

char buffer[] = "Quiero enviar un mensaje por el puerto serie\r\n";
char buffer_rec[512];
char aux[512];
osThreadId main_id;

int main(void){
	main_id = osThreadGetId();
	open(UART0, 115200, main_id);
	write(UART0, buffer, main_id);
	
	while(1){
		read(UART0, buffer_rec, main_id);
		
		if(*buffer_rec != NULL){
			if(strlen(aux) != strlen(buffer_rec))
				write(UART0, buffer_rec, main_id);
			memcpy(aux,buffer_rec,strlen(buffer_rec));
			//memset(buffer_rec,0,strlen(buffer_rec));
		}
	}
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
