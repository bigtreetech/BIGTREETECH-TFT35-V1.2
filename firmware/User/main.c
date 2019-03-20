#include "includes.h"


QUEUE infoCmd;     //待发送的命令
HOST  infoHost;    //主机的状态

MENU infoMenu;     //菜单结构体


void Hardware_Config(void)
{
    Delay_init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM4_Config(9999,71);								//系统时钟 10ms
	XPT2046_Init();
	W25Qxx_Init();
    
	LCD_Init();
    LCD_DMA_Config();
    
    LOGO_ReadDisplay();
       
    if(readStoredPara() == false)
    {    
        TSC_Calibration();
        infoSettings.baudrate = 115200;
        infoSettings.language = ENGLISH;
        storePara();
    }
    USART1_Config(infoSettings.baudrate);
    
    infoMenu.cur = 0;
    infoMenu.menu[infoMenu.cur] = menuMain;
    
    
    u32 startUpTime = OS_GetTime();
    heatSetUpdateTime(100);
    while(OS_GetTime() - startUpTime < 300)                //显示3秒钟的logo再进入主界面
    {                                                                                                                                            
        loopProcess();	
    }
    heatSetUpdateTime(300);
}



int main(void)
{	
	SCB->VTOR =FLASH_BASE|0x6000;
		
    Hardware_Config();
    
    for(;;)
    {
        (*infoMenu.menu[infoMenu.cur])();
    }
}
