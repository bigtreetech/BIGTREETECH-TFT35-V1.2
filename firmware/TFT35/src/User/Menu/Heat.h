#ifndef _HEAT_H_
#define _HEAT_H_

#include "stdint.h"
#include "stdbool.h"

typedef enum
{
  NOZZLE0=0,
  BED,
  TOOL_NUM,
}TOOL;


typedef struct
{
  int16_t current[TOOL_NUM],target[TOOL_NUM];
  bool    waiting[TOOL_NUM];
  TOOL    tool;
}HEATER;

void menuHeat(void);

void heatSetTargetTemp(TOOL tool, uint16_t temp);
uint16_t heatGetTargetTemp(TOOL tool);
void heatSetCurrentTemp(TOOL tool, int16_t temp);
int16_t heatGetCurrentTemp(TOOL tool);
bool heatGetIsWaiting(TOOL tool);
bool heatHasWaiting(void);
void heatSetIsWaiting(TOOL tool,bool isWaiting);
void heatSetCurrentTool(TOOL tool);
TOOL heatGetCurrentTool(void);
void heatSetUpdateTime(uint32_t time);
void heatSetUpdateWaiting(bool isWaiting);
void heatSetSendWaiting(TOOL tool, bool isWaiting);
void loopCheckHeater(void);

#endif

