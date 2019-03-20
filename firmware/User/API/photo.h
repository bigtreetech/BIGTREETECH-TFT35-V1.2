#ifndef _PHOTO_H_
#define _PHOTO_H_
#include "stm32f10x.h"
#include "GUI.h"




void LCD_DMA_Config(void);

void LOGO_ReadDisplay(void);
void ICON_ReadDisplay(u16 sx,u16 sy, u8 icon);
void ICON_PressedDisplay(u16 sx,u16 sy, u8 icon);

#endif

