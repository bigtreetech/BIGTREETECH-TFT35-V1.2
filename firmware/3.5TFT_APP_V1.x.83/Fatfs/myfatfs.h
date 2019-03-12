#ifndef _MYFATFS_H_
#define _MYFATFS_H_
#include "stm32f10x.h"
#include "includes.h"

void Scan_Files(void);
void Print_SdFile(const TCHAR* path,u32 lseek);

#endif 




















