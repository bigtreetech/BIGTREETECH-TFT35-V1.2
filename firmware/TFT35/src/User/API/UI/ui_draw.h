#ifndef _UI_DRAW_H_
#define _UI_DRAW_H_

#include "stdint.h"

#define ICON_WIDTH  95
#define ICON_HEIGHT 95

void LCD_DMA_Config(void);

void LOGO_ReadDisplay(void);
void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon);
void ICON_PressedDisplay(uint16_t sx, uint16_t sy, uint8_t icon);

#endif

