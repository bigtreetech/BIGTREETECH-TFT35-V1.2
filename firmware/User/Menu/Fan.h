#ifndef _FAN_H_
#define _FAN_H_
#include "includes.h"

void menuFan(void);

void fanSetSpeed(u8 speed);
u8   fanGetSpeed(void);
void fanSetSendWaiting(bool isWaiting);

#endif
