#include "LPC17xx.h"
#include "cmsis_os.h"
#include "lcddriver.h"

#ifndef __LCD_H
#define __LCD_H

typedef struct OS_LCD {
	uint8_t select;
	uint16_t x;
	uint16_t y;
	uint16_t x1;
	uint16_t y1;
	char c;
	char *s;
	uint16_t color;
	uint16_t bcolor;
	uint8_t size;
	uint16_t radio;
	uint16_t width;
	uint16_t height;
	uint16_t length;
	uint8_t rotflag;
}*P_LCD;

/*SYNCHRONIZATION BETWEEN THREADs*/
typedef struct L_SYNC_OS{
	unsigned char ID;
}*P_LSYNC;

extern P_LCD __svc(9) open_lcd(uint16_t color,uint8_t rotation, osThreadId ID);
extern void __svc(10) write_lcd(P_LCD lcd, osThreadId ID);
extern void __svc(11) close_lcd(osThreadId ID);


/*USE THIS TO SELECT OPTION TO VARIABLE "SELECT"*/
#define DRAW_CHAR 		0
#define DRAW_STRING 	1
#define DRAW_CIRCLE 	2
#define FILL_CIRCLE 	3
#define FILL_SCREEN 	4
#define DRAW_RECT 		5
#define	FILL_RECT			6
#define	DRAW_FLINE		7
#define	DRAW_LINE			8
#define	DRAW_PIXEL		9

#endif
