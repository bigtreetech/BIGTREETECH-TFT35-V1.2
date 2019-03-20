#include "usart.h"
#include "includes.h"



static const char errormagic[]  = "Error:";
static const char echomagic[]   = "echo:";

static char DMA_Rec_Buf[MAX_ACK_SIZE];

static u16 code_index=0;

static char str_seen(const char *str)
{
	u16 i;	
	for(code_index=0;DMA_Rec_Buf[code_index]!=0 && code_index < MAX_ACK_SIZE ;code_index++)
	{
		for(i=0; DMA_Rec_Buf[code_index+i]==str[i] && str[i]!=0 && DMA_Rec_Buf[code_index+i]!=0;i++)
        {}
        if(str[i]==0)
        {
            code_index+=i;      
            return true;
        }
	}
	return false;
}
static char str_cmp(const char *str1, const char *str2)
{
	u8 i;
	for(i=0;str1[i]!=0 && str2[i]!=0 && i<255;i++)
	{
		if(str1[i]!=str2[i])
			return false;
	}
	if(str1[i]!=0) return false;
	return true;
}


static float code_value()
{
	return (strtod(&DMA_Rec_Buf[code_index],NULL));
}




void USART1_Config(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC->AHBENR  |= 1<<0; //开启 DMA1EN 时钟
    RCC->APB2ENR |= 1<<2; //开启 PORTA 时钟
    RCC->APB2ENR |= 1<<14; //开启 USART1 时钟

    GPIOA->CRH &= 0xFFFFF00F;
    GPIOA->CRH |= 0x000008B0;// PA9 复用输出   PA10上拉输入

    //配置中断通道以及优先级
    NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);	

    USART1->BRR=(72*1000000)/(bound);// 波特率设置	 
    USART1->SR  &= ~(1<<4);           //清除空闲中断
    USART1->CR1 |=0X201C;  	         //使能USART1. 使能空闲中断 1位停止,无校验位.
    USART1->CR3 |= 1<<6;	           //使能串口接收DMA	

    USART1_RXDMA_CHANNEL->CPAR = (u32)(&USART1->DR);		//外设地址
    USART1_RXDMA_CHANNEL->CMAR = (u32)DMA_Rec_Buf;			//目标地址
    USART1_RXDMA_CHANNEL->CNDTR = MAX_ACK_SIZE;					//传输数据量	
    USART1_RXDMA_CHANNEL->CCR=0X00000000;	//复位
    USART1_RXDMA_CHANNEL->CCR |= 3<<12;   //11：通道优先级最高
    USART1_RXDMA_CHANNEL->CCR |= 1<<7;    //1：执行存储器地址增量操作
    USART1_RXDMA_CHANNEL->CCR |= 1<<0;    ////使能DMA		
}


/*
	函数功能：重新配置DMA空间
*/
void USART1_DMAReEnable(void)
{
    memset(DMA_Rec_Buf,0,MAX_ACK_SIZE);
    USART1_RXDMA_CHANNEL->CCR   &= ~(1<<0);
    USART1_RXDMA_CHANNEL->CNDTR = MAX_ACK_SIZE;  
    USART1_RXDMA_CHANNEL->CCR   |= 1<<0;    //使能DMA			
}

void USART1_IRQHandler(void)
{
	u16 DMA_bufrx=0;
	
	if((USART1->SR & (1<<4))!=0)
	{
		USART1->SR = ~(1<<4);
		USART1->DR;   //DMA+串口空闲中断  中断服务函数需加上这一句 读数据寄存器，不然会出现意料之外情况。
		
		DMA_bufrx=MAX_ACK_SIZE-USART1_RXDMA_CHANNEL->CNDTR;
		
		if(DMA_Rec_Buf[DMA_bufrx-1]=='\n')
		{
			infoHost.rx_ok=true;
		}
		else if(DMA_bufrx>MAX_ACK_SIZE-5)
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


void parseReceiveAck(void)
{
	if(infoHost.rx_ok !=true) return;   /* 未收到应答数据 */
	 
	if(str_cmp(DMA_Rec_Buf,"ok\r\n") || str_cmp(DMA_Rec_Buf,"ok\n"))
	{
		infoHost.wait=false;	
	}
	else
	{
		if(str_seen("ok"))
		{
			infoHost.wait=false;		
			if(infoHost.connected==false)
			{
				infoHost.connected=true;
			}
		}					
		if(str_seen("T:")) 
        {
			heatSetCurrentTemp(NOZZLE0,code_value()+0.5);
			if(str_seen("B:"))					
            {
				heatSetCurrentTemp(BED,code_value()+0.5);
			}
		}
		else if(str_seen("B:"))		
        {
			heatSetCurrentTemp(BED,code_value()+0.5);
		}
        else if(infoMenu.menu[infoMenu.cur] != menuPopup)
        {
            if(str_seen(errormagic))
            {
                popupSetContext((u8* )errormagic, (u8 *)DMA_Rec_Buf + code_index, textSelect(LABEL_CONFIRM), NULL);
                infoMenu.menu[++infoMenu.cur] = menuPopup;
            }
            else if(infoHost.connected && str_seen(echomagic))
            {
                popupSetContext((u8* )echomagic, (u8 *)DMA_Rec_Buf + code_index, textSelect(LABEL_CONFIRM), NULL);
                infoMenu.menu[++infoMenu.cur] = menuPopup;
            }    
        }
	}
    
	infoHost.rx_ok=false;
	USART1_DMAReEnable();
}









