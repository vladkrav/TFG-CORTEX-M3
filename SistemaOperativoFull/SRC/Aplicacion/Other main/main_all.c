#include "cmsis_os.h"
#include "LPC17xx.h"
#include "string.h"
#include "uartn.h"
#include "FILE_OS.h"
#include "TouchPanel_OS.h"
#include "LCD_h.h"
#include "easyweb.h"

void uart_thread(void const *argument);
void ethernet_thread(void const *argument);
void lcd_thread(void const *argument);
void touch_thread(void const *argument);
void file_thread(void const *argument);

osThreadId uart_id;
osThreadId ethernet_id;
osThreadId lcd_id;
osThreadId touch_id;
osThreadId file_id;

osThreadDef(uart_thread, osPriorityNormal, 1, 0);
osThreadDef(ethernet_thread, osPriorityNormal, 1, 0);
osThreadDef(lcd_thread, osPriorityNormal, 1, 0);
osThreadDef(touch_thread, osPriorityNormal, 1, 0);
osThreadDef(file_thread, osPriorityNormal, 1, 0);
int aux, aux_eth;
char buffer_uart[512];
char buffer_aux[] = "next thread\r";
unsigned char buffer_ethernet[256];
unsigned char buffer_read_eth [] = "Ejecutar la siguiente caracteristica\r";
P_TETH ethernet;
P_LCD s_lcd;
char path [512] ="0:";
char text[] = "FIRST FILE FROM OPERATIVE SYSTEM RTX\r\n";
char file[] = "0:/FILE.TXT";
char directory[] = "0:/CARPETA";
char file_Dir[] = "0:/CARPETA/FILE_DIR.TXT";
char file_text[] = "FILE_DIR IN A DIRECTORY CALLED CARPETA \r\n";

/*----------------------------------------------------------------------------
 *   UART Thread
 *---------------------------------------------------------------------------*/
void uart_thread(void const *argument){
	open_uart(UART0,115200,uart_id);
	write_uart(UART0, "Hola UART0,comprobando el funcionamiento del puerto serie en SO RTX\n", uart_id);
	while(1){
		read_uart(UART0, buffer_uart,uart_id);
		if(*buffer_uart != NULL){
			aux = strcmp(buffer_uart, buffer_aux);
			if(aux == 0){
				osSignalSet(ethernet_id, 0x01);
				close_uart(uart_id);
				//osSignalSet(lcd_id, 0x01);
				osSignalWait(0x01, osWaitForever);
			}
			write_uart(UART0, buffer_uart, uart_id);
			osDelay(500);
			memset(buffer_uart, 0, strlen(buffer_uart));
		}
	}
}
/*----------------------------------------------------------------------------
 *   Ethernet Thread
 *---------------------------------------------------------------------------*/
void ethernet_thread(void const *argument){
	//write_uart(UART0, "Ejecutando el periferico Ethernet \n", uart_id);
	osSignalWait(0x01, osWaitForever);
	ethernet = open_ethernet(ethernet_id);
	ethernet->IP_1 = 192;
	ethernet->IP_2 = 168;
	ethernet->IP_3 = 1;
	ethernet->IP_4 = 135;
	ethernet->S_C = SERVER;
	ethernet->P_EXT = 6000;
	ethernet->P_INT = 80;
	ethernet->n_con = 0;
	
	while(1){
		connect_ethernet(ethernet,ethernet_id,0);
		write_ethernet("Configurado el periferico de la comunicacion Ethernet\n", ethernet_id);
		read_ethernet(buffer_ethernet, ethernet_id);
		if(strcmp(buffer_ethernet, buffer_read_eth) == 0){
			osSignalSet(file_id, 0x01);
			close_ethernet(ethernet_id, ethernet);
			osDelay(500);
			osSignalWait(0x01, osWaitForever);
		}		
	}
}
/*----------------------------------------------------------------------------
 *   LCD Thread
 *---------------------------------------------------------------------------*/
void lcd_thread(void const *argument){
	osSignalWait(0x01, osWaitForever);
	s_lcd = open_lcd(RED,3,lcd_id);
	s_lcd-> select = FILL_CIRCLE;
	s_lcd-> x = 120;
	s_lcd-> y = 160;
	s_lcd-> x1 = 239;
	s_lcd-> y1 = 319;
	s_lcd-> c = 'A';
	s_lcd-> s = "Hello World";
	s_lcd-> color = YELLOW;
	s_lcd-> bcolor = YELLOW;
	s_lcd-> size = LARGE;
	s_lcd-> radio = 100;
	s_lcd-> width = 50;
	s_lcd-> height = 50;
	s_lcd-> length = 120;
	s_lcd-> rotflag = 1;
	write_lcd(s_lcd, lcd_id);
	osDelay(7500);
	osSignalSet(touch_id, 0x01);
	close_lcd(lcd_id);
	osSignalWait(0x01, osWaitForever);
	while(1);
}
/*----------------------------------------------------------------------------
 *   Touch Thread
 *---------------------------------------------------------------------------*/
void touch_thread(void const *argument){
	osSignalWait(0x01, osWaitForever);
	open_touch(touch_id);
	while(1){
		write_touch(touch_id);
 	}
}
/*----------------------------------------------------------------------------
 *   File Thread
 *---------------------------------------------------------------------------*/
void file_thread(void const *argument){
	osSignalWait(0x01, osWaitForever);
	open_file(file_id);
	write_file(NEW_FILE, text, file, file_id); //creacion de un archivo nuevo 
	write_file(MKDIR, 0, directory, file_id);	//creacion de una carpeta nueva
	write_file(NEW_FILE, file_text, file_Dir, file_id); //creacion de un archivo nuevo en la carpeta
	read_file(READ_FILE, path, file, file_id); //lectura de lo escrito en el primer archivo
	read_file(READ_FILE, path, file_Dir, file_id); //lectura de lo escrito en el segundo archivo
	read_file(SHOW_FILES,path,0,file_id); // muestra todos los archivos
	close_file(FREE_MEM,0,file_id);
	osDelay(7500);
	osSignalSet(lcd_id, 0x01);
	osSignalWait(0x01, osWaitForever);
	while(1);
	
}
/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void){
	/*CREACION DE HILOS*/
	uart_id = osThreadCreate(osThread(uart_thread), NULL);
	ethernet_id = osThreadCreate(osThread(ethernet_thread), NULL);
	lcd_id = osThreadCreate(osThread(lcd_thread), NULL);
	touch_id = osThreadCreate(osThread(touch_thread), NULL);
	file_id = osThreadCreate(osThread(file_thread), NULL);
	
	while(1){
		osSignalWait(0x01, osWaitForever);
	}
}
