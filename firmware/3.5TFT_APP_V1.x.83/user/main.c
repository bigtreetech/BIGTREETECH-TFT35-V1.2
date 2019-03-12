#include "includes.h"


bool 	press=false;
u8 		err_flag=0;
u8 		outage_save=0;

INFOHEATUPDATE infoHeatUpdate={300,false,false};

INFOHEAT			infoHeat={0,0,DEFAULT_NOZZLE_TEMP,DEFAULT_BED_TEMP,NOZZLE,5};		//cur_nozzle，cur_bed，tag_nozzle，tag_bed，type ,heat_unit
INFOMOVE 				infoMove={1};																		//move_unit
INFOEXTRUDE			infoExtrude={0,EXTRUDE_NORMAL_SPEED,5};				//len_totel,speed_unit,move_unit
INFOFAN					infoFan={0};																			//fan_speed
INFOSETTING			infoSetting={ENGLISH,115200};												//language
INFOPRINT				infoPrint={0,0,0};							//file,dispfile,back_icon

INFOPRINTING	  infoPrinting={0,0,100,100,1,5,1,0,false,false};	//time,baby_step,xyz_speed,e_speed,speed_type,speed_unit,baby_unit,pause,wait,ok
INFOMYFILE			infoMyfile={0,0,0,0,0,0,0};			//title,cur_nesting,folder,file,Fnum,fnum,cur_page

 
USART1_CMD 			infoCMD={{0},0,0,0};
USART1_FLAG	 		infoUSART1_CMD={0,0,0};

#define DEBUG	1

char str[]={
0x0F,0xF8,0x00,0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,
0x02,0x00,0x00,0x02,0x00,0x00,0x04,0x00,0x0F,0xF8,0x00,0x00,0x04,0x00,0x00,0x02,
0x00,0x00,0x00,0x00,};

void GUI_Disp1Char(u16 sx,u16 sy,u8 mode)
{  		
	u8 		x,y,i=0;
	u8		*font=str;
	u32 	offset=0;					//24*12/8 = 36
	u32 	temp;

	for(x=0;x<12;x++)
	{   
		temp=(font[i++]<<16)|(font[i++]<<8)|font[i++];
		
		for(y=0;y<24;y++)
		{			    
			if(temp&0x800000)GUI_DrawPixel(sx,sy+y,WHITE);
			else if(mode!=0)GUI_DrawPixel(sx,sy+y,BLACK);
			temp<<=1;
		} 
		sx++;
	}  	
   	 	  
}

int main(void)
{	
//	SCB->VTOR =FLASH_BASE|0x6000;
	
	Delay_init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	TIM4_Config(9999,71);								//系统时钟 10ms


	LCD_Init();
	W25QXX_Init();
#if DEBUG==0
	Read_Dispaly_Logo();		//显示logo
	Delay_ms(1000);
#endif
	
	infoSetting.language=*((vu16*)LANGUAGE_ADDR);
	if(infoSetting.language<TOTEL_LANGUAGE)
	{}else{infoSetting.language=ENGLISH;}
	
	infoSetting.baudrate=*((vu32*)(LANGUAGE_ADDR+4));
	if(infoSetting.baudrate==250000)
		infoSetting.baudrate=250000;
	else
		infoSetting.baudrate=115200;
	
	TP_GPIOConfig();		
	Read_Lcd_xy();					//从内部flash中读取触摸屏的校准系数
	
	USART1_Config(infoSetting.baudrate);	
	
	
#if DEBUG==0
	Delay_ms(1000);
#else
	infoUSART1_CMD.link_flag=1;
#endif
	storeCmd("M105\n");
	loopProcess();

	LCD_Clear(BLACK);

//    GUI_DispString(0,0,"\xA1\x40 \xA1\x41 \xA1\x42 \xA1\x43 \xA1\x44 \xA1\x45 \xA1\x46",0);
//    GUI_Disp1Char(0,100,1);
//    while(1);
	mainMenu();	
}

