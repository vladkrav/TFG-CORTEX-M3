/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The File System application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "ff.h"
#include "SPI_MSD_Driver.h"

#include <stdio.h>
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private variables ---------------------------------------------------------*/
FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc;         /* file objects */   
FRESULT res;
UINT br;

char path[512]="0:";
uint8_t textFileBuffer[] = "Thank you for using LPC1768-Mini-DK Development Board^_^ \r\n";   

/* Private function prototypes -----------------------------------------------*/
int SD_TotalSize(void);
void USART_Configuration(void);
FRESULT scan_files (char* path);


/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 	 : None
*******************************************************************************/
int main(void){
	MSD_SPI_Configuration();

	debug_frmwrk_init();

	printf("******************************************************************\n");
  printf("*                                                                *\n");
	printf("*   Thank you for using LPC1768-Mini-DK Development Board  ^_^   *\n");
	printf("*                                                                *\n");
	printf("******************************************************************\n");

	if( _card_insert() == 0 )
    {
	  printf("-- SD card detected OK \r\n");
    }
    else
    {
      printf("-- Please insert a SD card \r\n");
      while( _card_insert() != 0 );
      printf("-- SD card connection detected \r\n");
	  Delay(0xffffff);
    }

	f_mount(0,&fs);	

	res = f_open( &fsrc , "0:/Demo.TXT" , FA_CREATE_NEW | FA_WRITE);		

    if ( res == FR_OK )
    { 
      /* Write buffer to file */
      res = f_write(&fsrc, textFileBuffer, sizeof(textFileBuffer), &br);     
 
	  printf("Demo.TXT successfully created        \r\n");
    
      /*close file */
      f_close(&fsrc);      
    }
    else if ( res == FR_EXIST )
    {
	  printf("Demo.TXT created in the disk      \r\n");
    }

	scan_files(path);
	SD_TotalSize();

    /* Infinite loop */
    while (1)
	{
    }
}

/*******************************************************************************
* Function Name  : scan_files
* Description    : 
* Input          : - path:
* Output         : None
* Return         : FRESULT
* Attention		   : 
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : SD_TotalSize
* Description    : 
* Input          : None
* Output         : None
* Return         :
* Attention		 : None
*******************************************************************************/
int SD_TotalSize(void){
    FATFS *fs;
    DWORD fre_clust;        

    res = f_getfree("0:", &fre_clust, &fs);  
    if ( res==FR_OK ) 
    {
	  /* Print free space in unit of MB (assuming 512 bytes/sector) */
      printf("\r\n%d MB total drive space.\r\n"
           "%d MB available.\r\n",
           ( (fs->n_fatent - 2) * fs->csize ) / 2 /1024 , (fre_clust * fs->csize) / 2 /1024 );
		
	  return ENABLE;
	}
	else 
	  return DISABLE;   
}	 

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE{
	/* wait for current transmission complete - THR must be empty */
	while (UART_CheckBusy(DEBUG_UART_PORT) == SET);
	
	UART_SendByte(DEBUG_UART_PORT, ch);
	
	return ch;
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
