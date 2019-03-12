/****************************************************************************************
* 文件名：GUI_BASIC.H
* 功能：GUI基本画图函数。
* 说明：
****************************************************************************************/
#ifndef  _GUI_BASIC_H_
#define  _GUI_BASIC_H_

#include "stm32f10x.h"


enum
{
	LEFT=0,
	RIGHT,
};
			
typedef struct
{
	u16 x0,y0,x1,y1;
}GUI_RECT;			
				

typedef struct
{
	u8 flag;
	GUI_RECT range;
}RANGE_FONT;

				
void GUI_SetColor(u16 color);
void GUI_SetBKColor(u16 color);
void GUI_SetRange(u16 x0, u16 y0, u16 x1, u16 y1);
void GUI_CancelRange(void);
void GUI_DrawPixel(u16 x,u16 y,u16 color);

void GUI_DispChar(u16 sx,u16 sy,u16 ch, u8 mode);
void GUI_DispHz(u16 sx,u16 sy,u8 *hz, u8 mode);
void GUI_DispString(u16 x,u16 y,u8 *p, u8 mode);
void GUI_DispLenString(u16 x,u16 y,u8 *p,u8 mode,u8 len);
void Show_warning(u16 backColor,u16 color,  u8 *chs);


void Show_num(u16 x, u16 y,int inum ,u8 len,u8 leftOrRight,u8 mode);
void Show_Float(u16 x, u16 y,float num,u8 llen,u8 rlen);
void Show_Clock(u16 x, u16 y,int time);
void Show_Baby(u16 x, u16 y,float step);

																			//画点
void  GUI_HLine(u16 x0, u16 y0, u16 x1, u16 color);															//画横线
void  GUI_RLine(u16 x0, u16 y0, u16 y1, u16 color);															//画竖线
void  GUI_DrawRect(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);									//画矩形
void  GUI_FillRect(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);		//画有填充颜色的矩形

void  GUI_CircleFill(u16 x0, u16 y0, u8 r,u16 color);



#endif

