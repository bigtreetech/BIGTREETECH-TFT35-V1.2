#ifndef _OUTAGE_H_
#define _OUTAGE_H_

#include "includes.h"

void pauseCreate(void);
void resumeDelete(void);

bool printPause(void);
void printResume(bool flag);

void Outage_Config(void);
bool isOutage(void);
bool hasOutage(void);
bool outageMenu(void);


char *outageQueueP(u8 rank);
u8 outageQueueCount(void);

typedef struct
{
	float x,y,z,e,f;
	u32		offset;
	u16		nozzle,bed,fan;
	char 	path[_MAX_LFN];	
} OUTAGE;

extern OUTAGE infoOutage;

#define OUTAGE_FILE		"0:printing.sys"

#endif









