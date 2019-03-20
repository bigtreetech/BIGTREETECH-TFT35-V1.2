#ifndef _LCD_H_
#define _LCD_H_

#include "stm32f10x.h"
#include "Configration.h"


typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
} LCD_TypeDef;
															
#define LCD_BASE        ((u32)(0x60000000 | 0x00FFFFFE))  	//1111 1111 1111 1111 1111 1110	
#define LCD             ((LCD_TypeDef *) LCD_BASE)

#define LCD_WR_REG(regval) LCD->LCD_REG=regval
#define LCD_WR_DATA(data)  LCD->LCD_RAM=data


#if HARDWARE_VERSION == V1_2
	#define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(c); }while(0)
#elif HARDWARE_VERSION == V1_1 ||  HARDWARE_VERSION == V1_0 
	#define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(((c)>>8)&0xFF); LCD_WR_DATA((c)&0xFF); }while(0)	
#endif


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色


void LCD_Init(void);                                    //初始化
void LCD_Clear(u16 Color);                              //清屏
void LCD_DrawPoint(u16 x,u16 y,u16 c);                  //画点

#endif
