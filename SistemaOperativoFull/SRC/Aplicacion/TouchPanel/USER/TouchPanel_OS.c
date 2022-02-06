#include "LPC17xx.h"
#include <string.h>
#include "rt_TypeDef.h"
#include "RTX_config.h"
#include "GLCD.h"
#include "TouchPanel.h"
#include "TouchPanel_OS.h"
#include "rt_MemBox.h"

	/*MEMORY RESERVE*/
	P_TSYNC tsync;
	_declare_box(touch_pool,4,4); //Modify for requeriments
	unsigned int t_reserved = 0; //memory reserved for file struct
	/*FUNCTIONS PRIMITIVES*/
	P_TCB rt_tid2ptcb (osThreadId thread_id);
	
void __svc(12) open_touch(osThreadId ID);
void __SVC_12            (osThreadId ID){
	
	/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
	/*MEMORY RESERVE FOR STRUCT*/
	if(t_reserved == 0){
		rt_init_box(touch_pool,sizeof(touch_pool),sizeof(struct T_SYNC_OS));
		tsync = rt_alloc_box(touch_pool);
		memset(tsync,0,sizeof(struct T_SYNC_OS));
		t_reserved = 1;
	}
	/*EXTRACTING ID OF THE THREAD*/
	id = rt_tid2ptcb(ID);
	/*RUNNING THE CONFIGURATION*/
	if((tsync->ID == id->task_id) || (tsync->ID == 0)){
		tsync->ID = id->task_id;
		TP_Init();
		LCD_Initializtion();
		TouchPanel_Calibrate();
	}
}
void __svc(13) write_touch(osThreadId ID);
void __SVC_13             (osThreadId ID){
	P_TCB id;
	id = rt_tid2ptcb(ID);
	if(tsync->ID == id->task_id){
			getDisplayPoint(&display,Read_Ads7846(),&matrix);
			TP_DrawPoint(display.x,display.y);
		}
}

void __svc(18) close_touch(osThreadId ID);
void __SVC_18             (osThreadId ID){
	P_TCB id;
	id = rt_tid2ptcb(ID);
	if(tsync->ID == id->task_id){
		memset(tsync,0,sizeof(struct T_SYNC_OS)); //Primero, limpiar el espacio
		rt_free_box(touch_pool,tsync); //Segundo liberar la memoria
		t_reserved = 0;
	}
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

