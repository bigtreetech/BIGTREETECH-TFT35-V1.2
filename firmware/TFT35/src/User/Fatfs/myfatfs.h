#ifndef _MYFATFS_H_
#define _MYFATFS_H_

#include "stdint.h"

char mountSDCard(void);
char scanPrintFilesFatFs(void);
char Get_NewestGcode(const char* path);

#endif 
