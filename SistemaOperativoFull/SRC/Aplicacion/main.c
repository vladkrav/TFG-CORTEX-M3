/*********************/
/* INCLUDEs & DEFINEs*/
/*********************/
#include "cmsis_os.h"
#include "LPC17xx.h"
#include "stdlib.h"
#include "FILE_OS.h"

/***********/
/*VARIABLES*/
/***********/
char path [512] ="0:";
char text[] = "FIRST FILE FROM VLAD OPERATIVE SYSTEM \r\n";
char name[] = "0:/VLAD2.TXT";
char text2[] = "SECOND FILE FROM KEIL uVision 5.26\r\n";
char name2[] = "0:/CARPETA/FILE2.h";
/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void){
	open();
	write(text, name);
	write(text2,name2);
	read(path);
	while(1){
	}
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
