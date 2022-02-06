#include "LPC17xx.h"
#include <string.h>
#include "rt_TypeDef.h"
#include "RTX_config.h"
#include "LCD_h.h"
#include "rt_Memory.h"
#include "rt_MemBox.h"

struct OS_LCD *lcd = NULL;
/*MEMORY RESERVE*/
	P_LSYNC lsync;
_declare_box(lcd_pool,220,1);
_declare_box(slcd_pool,4,4);
unsigned int l_reserved = 0; //memory reserved for file struct
P_TCB rt_tid2ptcb (osThreadId thread_id);

/* rotation can be 0, 1, 2 or 3 (3 is the default option)*/
//    (239,319)|----------------------|(000,319)   (000,000)|----------------------|(000,000)
//             |E@<-----------------@ |                     | @   @              @E|
//           Y |         ::           |                     | ^   ^              ^ |
//           | |         ::           |                     | |   |    ..        | |
//           | | @<-----------------@ |                     | |   |    ..        | |
//           V |                      |                     | |   |              | |
//             | @<-----------------@B|                     |B@   @              @ |
//    (239,000)|----------------------|(000,000)   (000,000)|----------------------|(000,000)
//
//                    rotation = 0                                rotation = 1
//
//
//                                                 ( x,  y )        x ---->
//             |----------------------|            (000,000)|----------------------|(000,319)
//             | @              @   @B|                     |B@----------------->@ |
//             | |              |   | |                   y |                      |
//             | |      ..      |   | |                   | | @----------------->@ |
//             | |      ..      |   | |                   | |          :           |
//             | V              V   V |                   v |          :           |
//             |E@              @   @ |                     | @----------------->@E|
//             |----------------------|            (239,000)|----------------------|(239,319)
//                    rotation = 2                              rotation = 3 (DEFAULT)
P_LCD __svc(9) open_lcd(uint16_t color,uint8_t rotation, osThreadId ID);
P_LCD __SVC_9          (uint16_t color,uint8_t rotation, osThreadId ID){
	/*VARIABLE FOR SYNCHRONIZATION*/
	P_TCB id;
	/*MEMORY RESERVE FOR STRUCT*/
	if(l_reserved == 0){
		rt_init_box(slcd_pool,sizeof(slcd_pool),sizeof(struct L_SYNC_OS));
		lsync = rt_alloc_box(slcd_pool);
		memset(lsync,0,sizeof(struct L_SYNC_OS));
		l_reserved = 1;
	}
	/*EXTRACTING ID OF THE THREAD*/
	id = rt_tid2ptcb(ID);
	/*RUNNING THE CONFIGURATION*/
	if((lsync->ID == id->task_id) || (lsync->ID == 0)){
		lsync->ID = id->task_id;
		lcdInitDisplay(); //INICIO DE LCD
		rt_init_box(lcd_pool,sizeof(lcd_pool),sizeof(struct OS_LCD));
		lcd = rt_alloc_box(lcd_pool);
		memset(lcd,0,sizeof(struct OS_LCD));
		fillScreen(color);
		setRotation(rotation);
	}
	return(lcd);
}
 
void __svc(10) write_lcd(P_LCD lcd, osThreadId ID);
void __SVC_10           (P_LCD lcd, osThreadId ID){
	P_TCB id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(lsync->ID == id->task_id){
		switch (lcd->select)
		{
			/*REPRESENTAR EN PANTALLA MENSAJES O SIMPLEMENTE LETRAS*/
			case DRAW_CHAR:
				drawChar(lcd->x, lcd->y, lcd->c, lcd->color, lcd->bcolor, lcd->size);
			break;
			case DRAW_STRING:
				drawString(lcd->x, lcd->y, lcd->s, lcd->color, lcd->bcolor, lcd->size);
			break;
			/*REPRESENTAR EN PANTALLA UN CIRCULO*/
			case DRAW_CIRCLE:
				drawCircle(lcd->x, lcd->y, lcd->radio, lcd->color);
			break;
			case FILL_CIRCLE:
				fillCircle(lcd->x, lcd->y, lcd->radio, lcd->color);
			break;
			/*RELLENAR LA PANTALLA DE UN COLOR ESPECIFICO*/
			case FILL_SCREEN:
				fillScreen(lcd->color);
			break;
			/*REPRESENTAR EN PANTALLA UNA RECTA*/
			case DRAW_RECT:
				drawRect(lcd->x, lcd->y, lcd->width, lcd->height, lcd->color);
			break; 
			case FILL_RECT:
				fillRect(lcd->x, lcd->y, lcd->width, lcd->height, lcd->color);
			break;
			/*REPRESENTAR EN PANTALLA UNA LINEA*/
			case DRAW_FLINE:
				drawFastLine(lcd->x,lcd->y,lcd->length,lcd->color, lcd->rotflag);
			break;
			case DRAW_LINE:
				drawLine(lcd->x, lcd->y, lcd->x1, lcd->y1, lcd->color);
			break;
			/*REPRESENTAR EN PANTALLA UN PIXEL DE UN COLOR ESPECIFICO*/
			case DRAW_PIXEL:
				drawPixel(lcd->x, lcd->y, lcd->color);
			break;
		}
	}
}
void __svc(11) close_lcd(osThreadId ID);
void __SVC_11           (osThreadId ID){
	P_TCB id;
	id = rt_tid2ptcb(ID); //extraemos la id del hilo
	if(lsync->ID == id->task_id){
		memset(lcd,0,sizeof(struct OS_LCD)); //Primero, limpiar el espacio
		memset(slcd_pool,0,sizeof(struct L_SYNC_OS)); //Primero, limpiar el espacio
		rt_free_box(lcd_pool,lcd); //Segundo liberar la memoria
		rt_free_box(slcd_pool,lsync); //Segundo liberar la memoria
		l_reserved = 0;
	}
}
