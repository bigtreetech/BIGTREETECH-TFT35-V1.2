#include "includes.h"
#include "timer.h"


void TIM4_Config(u16 psc,u16 arr)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
 
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
	TIM_ITConfig(TIM4,TIM_IT_Update ,ENABLE);
	TIM_Cmd(TIM4, ENABLE);  
}
u32 os_time=0;
void TIM4_IRQHandler(void)   //TIM4中断
{
	static bool touch=false;
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		os_time++;
//获取触摸值
		if(!TPEN())
		{
			if(touch)
			{
				press=true;
			}
			else
			{
				touch=true;
			}
		}
		else
		{
			press=false;
			touch=false;
		}
//定时发送M105查询温度	
		if((os_time%infoHeatUpdate.get_time==0) && !infoHeatUpdate.update && !infoHeatUpdate.wait_M105)
		{
			infoHeatUpdate.update=true;
		}
//打印过程中统计打印耗时		
		if(os_time%100==0)
		{
			if(infoPrinting.ok&&!infoPrinting.pause)
			{
				infoPrinting.time++;
				outage_save |= 0x02;
			}
		}
//清除警告信息
		if(err_flag>1)
		{
			err_flag--;
//			if(err_flag==100)
//			{
//				err_flag=1;
//			}
		}
//计时溢出		
		if(os_time==(u32)(~0))
		{
			os_time=0;
		}
		
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
	}
}


u32 OS_GetTime(void)
{
	return os_time;
}






