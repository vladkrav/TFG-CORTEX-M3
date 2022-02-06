#include "LPC17xx.h"
#include "ff.h"
#include "cmsis_os.h"


#ifndef _FILE_OS
#define _FILE_OS

extern void __svc(14) open_file(osThreadId ID);
extern void __svc(15) write_file(uint8_t option, char *text, char *file, osThreadId ID);
extern void __svc(16) read_file(uint8_t option, char *pth, char *file, osThreadId ID);
extern void __svc(17) close_file(uint8_t option, const TCHAR *f_dir, osThreadId ID);

/*BASE PROYECT FUNCTIONS*/
extern void  Delay (uint32_t nCount);
extern int SD_TotalSize(void);
FRESULT scan_files (char* path);


/*USE DEFINEs TO CHOOSE OPTION FOR SVC write*/
#define NEW_FILE 		0
#define MKDIR 			1
#define OPEN_FILE   2
/*DEFINE FOR SVC READ FUNCTION */
#define READ_FILE		0
#define SHOW_SIZE		1
#define SHOW_FILES  2
/*DEFINE FOR SVC CLOSE FUNCTION*/
#define REMOVE 			0
#define UNMOUNT			1
#define FREE_MEM		2
/*SYNCHRONIZATION BETWEEN THREADs*/
typedef struct F_SYNC_OS{
	unsigned char ID;
}*P_FSYNC;

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
