#ifndef _USART1_H_
#define _USART1_H_
#include "stm32f10x.h"


#define MAX_ACK_SIZE 300
#define USART1_RXDMA_CHANNEL	DMA1_Channel5





void USART1_Config(u32 bound);
void USART1_Puts(char *s);

void USART_Putc(USART_TypeDef* USARTx, char data);
void parseReceiveAck(void);



#endif 




















