#ifndef _BSDPRINTING_H_
#define _BSDPRINTING_H_
//#include "stm32f10x.h"
#include "stdbool.h"
#include "Printing.h"
//#include "ff.h"

void menuBeforeBSDPrinting(void);
bool setBSDPrintPause(bool is_pause);
void menuBSDPrinting(void);
void haltBSDPrinting(void);
void menuStopBSDPrinting(void);

#endif
