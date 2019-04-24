#include "usart.h"


#define DMA_TRANS_LEN  ACK_MAX_SIZE

char *dma_mem_buf = ack_rev_buf;

void USART1_Config(u32 baud)
{
  float baud_div = 0.0f;
  u32   baud_brr = 0;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC->AHBENR  |= 1<<0; //���� DMA1EN ʱ��
  RCC->APB2ENR |= 1<<2; //���� PORTA ʱ��
  RCC->APB2ENR |= 1<<14; //���� USART1 ʱ��

  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000008B0;// PA9 �������   PA10��������

  //�����ж�ͨ���Լ����ȼ�
  NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);	

  baud_div = (72000000.0f/16.0f/baud);
  baud_brr = (u32)baud_div<<4;
  baud_brr += (u8)((baud_div-(u32)baud_div)*16 + 0.5);
  USART1->BRR = baud_brr;           // ����������	 
  USART1->SR  &= ~(1<<4);           //��������ж�
  USART1->CR1 |=0X201C;  	         //ʹ��USART1. ʹ�ܿ����ж� 1λֹͣ,��У��λ.
  USART1->CR3 |= 1<<6;	           //ʹ�ܴ��ڽ���DMA	

  DMA1_Channel5->CPAR = (u32)(&USART1->DR);		//�����ַ
  DMA1_Channel5->CMAR = (u32) dma_mem_buf;			//Ŀ���ַ
  DMA1_Channel5->CNDTR = DMA_TRANS_LEN;         //����������	
  DMA1_Channel5->CCR = 0X00000000;	//��λ
  DMA1_Channel5->CCR |= 3<<12;   //11��ͨ�����ȼ����
  DMA1_Channel5->CCR |= 1<<7;    //1��ִ�д洢����ַ��������
  DMA1_Channel5->CCR |= 1<<0;    //ʹ��DMA		
}


/*
	�������ܣ���������DMA�ռ�
*/
void USART1_DMAReEnable(void)
{
  memset(dma_mem_buf,0,DMA_TRANS_LEN);
  DMA1_Channel5->CCR &= ~(1<<0);
  DMA1_Channel5->CNDTR = DMA_TRANS_LEN;  
  DMA1_Channel5->CCR |= 1<<0;    //ʹ��DMA			
}

void USART1_IRQHandler(void)
{
  u16 rx_len=0;

  if((USART1->SR & (1<<4))!=0)
  {
    USART1->SR = ~(1<<4);
    USART1->DR;   //DMA+���ڿ����ж�  �жϷ������������һ�� �����ݼĴ�������Ȼ���������֮�������

    rx_len = DMA_TRANS_LEN - DMA1_Channel5->CNDTR;

    if(dma_mem_buf[rx_len-1]=='\n')
    {
      infoHost.rx_ok=true;
    }
    else if(rx_len > DMA_TRANS_LEN-5)
    {
      infoHost.rx_ok=true;
    }
  }
}

void USART1_Puts(char *s )
{
  while (*s)
  {
    while((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);
    USART1->DR = ((u16)*s++ & (uint16_t)0x01FF);
  }
}


//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}






