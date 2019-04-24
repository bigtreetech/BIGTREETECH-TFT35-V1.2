#ifndef _FAN_H_
#define _FAN_H_
#include "stdint.h"
#include "stdbool.h"

void menuFan(void);

void    fanSetSpeed(uint8_t speed);
uint8_t fanGetSpeed(void);
void    fanSetSendWaiting(bool isWaiting);

#endif
