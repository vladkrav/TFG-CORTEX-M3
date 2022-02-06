#include "LPC17xx.h"
#include "cmsis_os.h"


#ifndef _TOUCH
#define _TOUCH
extern void __svc(12) open_touch(osThreadId ID);
extern void __svc(13) write_touch(osThreadId ID);
extern void __svc(18) close_touch(osThreadId ID);

/*SYNCHRONIZATION BETWEEN THREADs*/
typedef struct T_SYNC_OS{
	unsigned char ID;
}*P_TSYNC;
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
