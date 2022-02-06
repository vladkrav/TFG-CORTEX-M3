#include "LPC17xx.h"
#include "cmsis_os.h"
#include "ff.h"
#include "SPI_MSD_Driver.h"
#include <stdio.h>
#include <string.h>
#include "rt_TypeDef.h"
#include "RTX_config.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "rt_MemBox.h"
#include "FILE_OS.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

	FATFS fs;         /* Work area (file system object) for logical drive */
	FIL fsrc;         /* file objects */   
	FRESULT res;
	UINT br;
	unsigned int size;
	/*MEMORY RESERVE*/
	P_FSYNC fsync;
	_declare_box(file_pool,10,10); //Modify for requeriments
	unsigned int f_reserved = 0; //memory reserved for file struct
	P_TCB rt_tid2ptcb (osThreadId thread_id);
	
void __svc(14) open_file(osThreadId ID);
void __SVC_14           (osThreadId ID){
	
	/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
	/*MEMORY RESERVE FOR STRUCT*/
	if(f_reserved == 0){
		rt_init_box(file_pool,sizeof(file_pool),sizeof(struct F_SYNC_OS));
		fsync = rt_alloc_box(file_pool);
		memset(fsync,0,sizeof(struct F_SYNC_OS));
		f_reserved = 1;
	}
	/*EXTRACTING ID OF THE THREAD*/
	id = rt_tid2ptcb(ID);
	/*RUNNING THE CONFIGURATION*/
	if((fsync->ID == id->task_id) || (fsync->ID == 0)){
		fsync->ID = id->task_id;
		MSD_SPI_Configuration();
	
		debug_frmwrk_init();
	
		printf("----------------------------------------------------------------\n");
		printf("                                                                \n");
		printf("   Thank you for using LPC1768-Mini-DK Development Board		 		\n");
		printf("                                                                \n");
		printf("----------------------------------------------------------------\n");

		if( _card_insert() == 0 ){
			printf("--> SD card detected OK\r\n");
		}
		else{
			printf("--> Please insert a SD card\r\n");
			while( _card_insert() != 0 );
			printf("--> SD card connection detected\r\n");
			//Delay(0xffffff);
		}
		f_mount(0,&fs);
	}
}
void __svc(15) write_file(uint8_t option, char *text, char *file, osThreadId ID);
void __SVC_15            (uint8_t option, char *text, char *file, osThreadId ID){
	/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(fsync->ID == id->task_id){
		switch(option)
		{
			case NEW_FILE:
				res = f_open(&fsrc, file, FA_CREATE_NEW | FA_WRITE);
				if ( res == FR_OK ){ 
					/* Write buffer to file */
					res = f_write(&fsrc, text, strlen(text), &br);     
 
					printf("-->File successfully created\r\n");
    
					/*close file */
					f_close(&fsrc);      
				}
				else if ( res == FR_EXIST ){
					printf("-->File created in the disk\r\n");
				}
				break;
			case MKDIR:
				res = f_mkdir(file);
				if( res == FR_OK)
					printf("-->Directory successfully created\r\n");
				else if(res == FR_EXIST)
					printf("-->There is a directory with the same name\r\n");
				else if(res == FR_DENIED)
					printf("-->No space to allocate a new cluster\r\n");
				break;
			case OPEN_FILE:
				res = f_open(&fsrc, file,FA_OPEN_EXISTING | FA_WRITE);
				if(res == FR_OK){
					res = f_write(&fsrc, text, strlen(text), &br);
					printf("-->Successfully written in a existing file\r\n");
					f_close(&fsrc);
				}
				else if( res == FR_EXIST)
					printf("-->File doesn't written\r\n");
				break;
		}
	}
}

void __svc(16) read_file(uint8_t option, char *pth, char *file, osThreadId ID);
void __SVC_16           (uint8_t option, char *pth, char *file, osThreadId ID){
	char data[512];
		/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(fsync->ID == id->task_id){
		switch(option)
		{
			case READ_FILE:
				res = f_open(&fsrc, file, FA_READ);
				res = f_read(&fsrc, data, 512, &br);
				f_close(&fsrc);
				printf("%s\r\n", data);
				break;
			case SHOW_SIZE:
				SD_TotalSize();
				break;
			case SHOW_FILES:
				scan_files(pth);
				break;
		}
	}
}

void __svc(17)close_file(uint8_t option, const TCHAR *f_dir, osThreadId ID);
void __SVC_17 		      (uint8_t option, const TCHAR *f_dir, osThreadId ID){
		/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(fsync->ID == id->task_id){
		switch(option)
		{
			case REMOVE:
				res = f_unlink(f_dir); //remove a File or Directory
				if(res == FR_OK)
					printf("-->Removed successfully\r\n");
				else if(res == FR_INVALID_NAME)
					printf("-->The path name format is invalid\r\n");
				else if(res == FR_DENIED)
					printf("-->Acces denied due to prohibited access or directory full\r\n");
				else if(res == FR_INT_ERR)
					printf("-->Assertion failed\r\n");
				break;
			case UNMOUNT:
				res = f_mount(0,NULL);
				if(res == FR_OK)
					printf("-->Unmounted successfully\r\n");
				else if(res == FR_INVALID_DRIVE)
					printf("-->The logical drive number is invalid\r\n");
				break;
			case FREE_MEM:
				memset(fsync,0,sizeof(struct F_SYNC_OS)); //Primero, limpiar el espacio
				rt_free_box(file_pool,fsync); //Segundo liberar la memoria
				f_reserved = 0;
			break;
		}
	}	
}
FRESULT scan_files (char* path){
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {
                printf("%s/%s \r\n", path, fn);
            }
        }
    }
    return res;
}

int SD_TotalSize(void){
    FATFS *fs;
    DWORD fre_clust;        

    res = f_getfree("0:", &fre_clust, &fs);
    if ( res==FR_OK ) 
    {
	  /* Print free space in unit of MB (assuming 512 bytes/sector) */
      printf("\r\n%lu MB total drive space.\r\n"
           "%lu MB available.\r\n",
           ((fs->n_fatent - 2) * fs->csize ) / 2 /1024 , (fre_clust * fs->csize) / 2 /1024 );
		
	  return ENABLE;
	}
	else 
	  return DISABLE;   
}	 

void  Delay (uint32_t nCount){
  for(; nCount != 0; nCount--);
}
PUTCHAR_PROTOTYPE{
	/* wait for current transmission complete - THR must be empty */
	while (UART_CheckBusy(DEBUG_UART_PORT) == SET);
	
	UART_SendByte(DEBUG_UART_PORT, ch);
	
	return ch;
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
