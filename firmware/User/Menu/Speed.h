#ifndef _SPEED_H_
#define _SPEED_H_

#include "includes.h"

void speedSetSendWaiting(u8 tool, bool isWaiting);
void speedSetPercent(u8 tool, u16 per);
u16  speedGetPercent(u8 tool);
void menuSpeed(void);

#endif
