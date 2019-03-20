#ifndef _GUI_H_
#define _GUI_H_

#include "stm32f10x.h"
#include "stdbool.h"

#define LCD_WIDTH	480
#define LCD_HEIGHT	320

#define BYTE_HEIGHT   24
#define BYTE_WIDTH   (BYTE_HEIGHT/2)

#define FK_COLOR	WHITE
#define BK_COLOR	BLACK

enum
{
	LEFT=0,
	RIGHT,
	CENTER,
};

typedef struct 
{
	s16 x0,y0,w,h;
}GUI_WIN;
typedef struct 
{
	s16 x,y;
}GUI_POINT;

typedef struct
{
	s16 x0,y0,x1,y1;
}GUI_RECT;

extern const u32 GUI_Pow10[10];
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_SetWindow(u16 sx,u16 sy,u16 ex,u16 ey);


void GUI_SetColor(u16 color);
void GUI_SetBkColor(u16 bkcolor);
u16  GUI_GetColor(void);
u16  GUI_GetBkColor(void);
void GUI_Clear(u16 color);
void GUI_DrawPoint(u16 x,u16 y);
void GUI_FillRect(u16 sx,u16 sy,u16 ex,u16 ey);
void GUI_ClearRect(u16 sx,u16 sy,u16 ex,u16 ey);
void GUI_FillRectColor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void GUI_FillRectArry(u16 sx,u16 sy,u16 ex,u16 ey,u8 *arry);
void GUI_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void GUI_HLine(u16 x1,u16 y,u16 x2);
void GUI_VLine(u16 x1,u16 y1,u16 y2);
void GUI_DrawRect(u16 x1, u16 y1, u16 x2, u16 y2);
void GUI_DrawCircle(u16 x0,u16 y0,u8 r);
void GUI_FillCircle(u32 x0, u32 y0, u32 r);

void GUI_SetRange(s16 x0, s16 y0, s16 x1, s16 y1);
void GUI_CancelRange(void);
void GUI_DrawPixel(s16 x,s16 y,u16 color);
void GUI_DispChar(s16 x,s16 y,u16 ch,u8 mode);
const u8* GUI_DispString(s16 x,s16 y,const u8 *p,u8 mode);
const u8* GUI_DispLenString(s16 x,s16 y,const u8 *p,u8 mode,u8 len);
void GUI_DispStringRight(s16 x, s16 y, const u8 *p,u8 mode);
void GUI_DispStringInRect(s16 sx,s16 sy,s16 ex, s16 ey, const u8 *p,u8 mode);
void GUI_DispStringInPrect(const GUI_RECT *rect, const u8 *p,u8 mode);
void GUI_DispCharInPrect(const GUI_RECT *rect, u16 p,u8 mode);

void GUI_DispDec(s16 x,s16 y,s32 num,u8 len,u8 mode,u8 leftOrRight);
void GUI_DispFloat(s16 x,s16 y,float num,u8 llen,u8 rlen);


/****************************************************     Widget    *******************************************************************/

#define RADIO_SIZE 5
typedef struct
{
	u8 *context[RADIO_SIZE];
	u16 sx;
	u16 sy;
	u8 distance;
	u8 num;
	u8 select;
}RADIO;

void RADIO_Create(RADIO *raido);
void RADIO_Select(RADIO *raido, u8 select);

typedef struct
{
    GUI_RECT *rect;
    u8   *text;
    u32  time;
    s16	 off_head;
    s16  off_tail; 
    u16  len_size,len_total,len_max;
    u16  index;
    bool has_disp; 
}SCROLL;
void Scroll_CreatePara(SCROLL * para, u8 *pstr ,GUI_RECT *rect);
void Scroll_DispString(SCROLL * para,u8 mode,u8 align);



typedef struct
{
    const GUI_RECT rect;
    const u8  *context;
    const u16 radius;
    const u16 lineWidth;
    const u16 lineColor;
    const u16 fontColor;
    const u16 backColor;
    const u16 pLineColor;
    const u16 pFontColor;
    const u16 pBackColor;
}BUTTON;

void GUI_DrawButton(const BUTTON *button, bool pressed);



typedef struct
{
    const u16 fontColor;
    const u16 backColor;
    const s16 height;
}WINDOW_ITEM;

typedef struct
{
    const GUI_RECT rect;
    const u16 radius;
    const u16 lineWidth;
    const u16 lineColor;
    const WINDOW_ITEM title;
    const WINDOW_ITEM info;
    const WINDOW_ITEM bottom;
}WINDOW;

void GUI_DrawWindow(const WINDOW *window, const u8 *title, const u8 *inf);

#endif

