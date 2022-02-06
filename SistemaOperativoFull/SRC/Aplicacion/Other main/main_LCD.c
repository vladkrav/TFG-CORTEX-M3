/*********************/
/* INCLUDEs & DEFINEs*/
/*********************/
#include "cmsis_os.h"
#include "LPC17xx.h"
#include "stdlib.h"
#include "LCD_h.h"

/***********/
/*VARIABLES*/
/***********/
uint8_t i=1;
/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void){
	osThreadId lcd_id;
	lcd_id = osThreadGetId();
	P_LCD s_lcd;
	s_lcd=open_lcd(WHITE,3,lcd_id);
	s_lcd->select = DRAW_PIXEL;
	s_lcd-> x = 120;
	s_lcd-> y = 160;
	s_lcd-> x1 = 239;
	s_lcd-> y1 = 319;
	s_lcd-> c = 'A';
	s_lcd-> s = "Hello World";
	s_lcd-> color = RED;
	s_lcd-> bcolor = YELLOW;
	s_lcd-> size = LARGE;
	s_lcd-> radio = 100;
	s_lcd-> width = 50;
	s_lcd-> height = 50;
	s_lcd-> length = 120;
	s_lcd-> rotflag = 1;
	
	write_lcd(s_lcd,lcd_id);
	while(1){
		if(i==0)
			close_lcd(lcd_id);
	}
}
