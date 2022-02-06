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
uint8_t i_=0;
char path [512] ="0:";
char text[] = "FIRST FILE IN OPERATIVE SYSTEM \r\n";
char file[] = "0:/FILE.TXT";
char text2[] = "SECOND FILE FROM KEIL uVision 5.26\r\n";
char directory[] = "0:/CARPETA";
char file_Dir[] = "0:/CARPETA/FILE_DIR.TXT";
char file_text[] = "FILE_DIR IN A DIRECTORY CALLED CARPETA \r\n";
/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void){
	osThreadId file_id;
	file_id = osThreadGetId();
	open_file(file_id);
	if(i_==1){
	write_file(NEW_FILE, text, file, file_id);
	write_file(MKDIR, 0, directory, file_id);
	write_file(NEW_FILE, file_text, file_Dir, file_id);
	read_file(READ_FILE, path, file, file_id);
	read_file(READ_FILE, path, file_Dir, file_id);
	read_file(SHOW_FILES,path,0,file_id);
	read_file(SHOW_SIZE, path,0,file_id);
	}
	else{
		close_file(REMOVE, file_Dir, file_id);
		close_file(REMOVE, directory, file_id);
		close_file(REMOVE, file, file_id);
		
		//read_file(SHOW_SIZE,path,0,file_id);
	}
	while(1);
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
