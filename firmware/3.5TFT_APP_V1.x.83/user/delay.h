#ifndef _DELAY_H_
#define _DELAY_H_
#include "includes.h"

void Delay_init(u8 clk); 		//初始化
void Delay_ms(u16 ms);				//延时毫秒级
void Delay_us(u32 us);				//延时微秒级
void Delay(int time);				//软件延时

#endif
