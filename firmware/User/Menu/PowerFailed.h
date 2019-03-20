#ifndef _POWERFAILED_H_
#define _POWERFAILED_H_

#include "stm32f10x.h"
#include "heat.h"
#include "coordinate.h"
#include "ff.h"



#define BREAK_POINT_FILE		"0:Printing.sys"


void menuPowerOff(void);

bool powerFailedCreate(char *path) ;
void powerFailedCache(u32 offset);
void powerFailedClose(void);
void powerFailedDelete(void);

void powerFailedEnable(bool enable);
bool powerFailedlSeek(FIL* fp);

#endif
