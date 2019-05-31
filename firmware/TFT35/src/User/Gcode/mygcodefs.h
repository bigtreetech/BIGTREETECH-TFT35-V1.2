#ifndef _MYGCODEFS_H_
#define _MYGCODEFS_H_

#include "stdint.h"
#include "gcode.h"

char mountGcodeSDCard(void);
char scanPrintFilesGcodeFs(void);
char Get_NewestGcodeGcodeFs(const char* path);

#endif 
