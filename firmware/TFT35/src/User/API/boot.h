#ifndef _BOOT_H_
#define _BOOT_H_

#include "stm32f10x.h"

#define BMP		(1<<1)
#define FONT	(1<<2)

#define BMP_ROOT_DIR "0:TFT35/bmp"
#define FONT_ROOT_DIR "0:TFT35/font"


typedef union
{
	u16 color;
	struct{
	u16  b:5;
	u16  g:6;
	u16  r:5;
 }RGB;
}GUI_COLOR;

void scanUpdates(void);

#endif 
