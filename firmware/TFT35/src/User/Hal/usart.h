#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"
#include "parseACK.h"

void USART1_DMAReEnable(void);
    
void USART1_Config(u32 baud);
void USART1_Puts(char *s);

#endif 




















