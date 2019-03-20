#ifndef _HEAT_H_
#define _HEAT_H_

#include "includes.h"

typedef enum
{
	NOZZLE0=0,
	BED,
	TOOL_NUM,
}TOOL;


typedef struct
{
	s16  current[TOOL_NUM],target[TOOL_NUM];
	bool waiting[TOOL_NUM];
	TOOL tool;
}HEATER;

void menuHeat(void);


void heatSetTargetTemp(TOOL tool,u16 temp);
u16  heatGetTargetTemp(TOOL tool);
void heatSetCurrentTemp(TOOL tool,s16 temp);
s16  heatGetCurrentTemp(TOOL tool);
bool heatGetIsWaiting(TOOL tool);
bool heatHasWaiting(void);
void heatSetIsWaiting(TOOL tool,bool isWaiting);
void heatSetCurrentTool(TOOL tool);
TOOL heatGetCurrentTool(void);
void heatSetUpdateTime(u32 time);
void heatSetUpdateWaiting(bool isWaiting);
void heatSetSendWaiting(TOOL tool, bool isWaiting);
void loopCheckHeater(void);

#endif

