#include "outage.h"
#include "includes.h"

OUTAGE infoOutage={0,0,0,0,0,0,0,0,0,{0}};


char cmdOutage[11][MAX_CMD_SIZE];
bool hasOutage(void)
{
	FIL 	fp;
	UINT	br;

	if(f_open(&fp,OUTAGE_FILE,FA_OPEN_EXISTING|FA_READ)==FR_OK)
	{
		if(f_read(&fp,&infoOutage,sizeof(infoOutage),&br)==FR_OK)
		{	
			sprintf(cmdOutage[0],"M109 S%d\n",infoOutage.nozzle);
			sprintf(cmdOutage[1],"G28\n");
			sprintf(cmdOutage[2],"M190 S%d\n",infoOutage.bed);
			sprintf(cmdOutage[3],"G28\n");
			sprintf(cmdOutage[4],"G21\n");
			sprintf(cmdOutage[5],"G90\n");
			sprintf(cmdOutage[6],"M106 S%d\n",infoOutage.fan);
			sprintf(cmdOutage[7],"G92 E0\n");
			sprintf(cmdOutage[8],"G1 E5.5\n");
			sprintf(cmdOutage[9],"G92 E%f\n",infoOutage.e);
			sprintf(cmdOutage[10],"G1 X%f Y%f Z%f F%f\n",infoOutage.x,infoOutage.y,infoOutage.z,infoOutage.f);
			f_close(&fp);
			return true;
		}
	}	
	return false;
}
u8 outageQueueCount(void)
{
	return (sizeof(cmdOutage)/sizeof(cmdOutage[0]));
}
char *outageQueueP(u8 rank)
{
	return cmdOutage[rank];
}

void outageMenuPic(void)
{
	u8 x_offset=0;
	
	cur_view=MENU_OUTAGE;
	
	
	menuPicClear();
	GUI_DispString(10,10,(u8 *)infoOutage.path,0);
	x_offset=my_strlen(textSelect(OUTAGE_INFO));
	if(x_offset>35)
	{	
		GUI_DispLenString((480-(x_offset*6))>>1 ,80,textSelect(OUTAGE_INFO),0,x_offset/2);
		GUI_DispLenString((480-(x_offset*6))>>1 ,110,textSelect(OUTAGE_INFO)+x_offset/2,0,x_offset/2);
	}else{
	x_offset=(480-(x_offset*12))>>1;
	GUI_DispString(x_offset ,100,textSelect(OUTAGE_INFO),0);
	}
	
	GUI_FillRect(menuRectOutage[0].x0,menuRectOutage[0].y0,menuRectOutage[0].x1,menuRectOutage[0].y1,GRAY);
	x_offset=(menuRectOutage[0].x1-menuRectOutage[0].x0-(my_strlen(textSelect(CONTINUE))*12))>>1;
	GUI_DispString(menuRectOutage[0].x0+x_offset ,menuRectOutage[0].y0+((menuRectOutage[0].y1-menuRectOutage[0].y0-24)>>1),textSelect(CONTINUE),0);	
	
	GUI_FillRect(menuRectOutage[1].x0,menuRectOutage[1].y0,menuRectOutage[1].x1,menuRectOutage[1].y1,GRAY);
	x_offset=(menuRectOutage[1].x1-menuRectOutage[1].x0-(my_strlen(textSelect(CANCEL))*12))>>1;
	GUI_DispString(menuRectOutage[1].x0+x_offset,menuRectOutage[0].y0+((menuRectOutage[1].y1-menuRectOutage[1].y0-24)>>1),textSelect(CANCEL),0);
}
bool outageMenu(void)
{	
	if(hasOutage())
	{	
		u8 key_num;
		outageMenuPic();
		while(1)
		{
			key_num=Get_Place(MENU_OUTAGE);
			if(infoUSART1_CMD.link_flag==1)
			{
				switch(key_num)
				{
					case 1:
						strcpy(infoPrint.file,infoOutage.path);
						memcpy(infoPrint.dispfile,infoOutage.path,sizeof(infoPrint.dispfile));						
						return true;
					case 2:	
						infoOutage.offset=0;
						f_unlink(OUTAGE_FILE);
						return false;
				}
			}else if(key_num!=0){
				infoOutage.offset=0;
				return false;
			}
				
			loopProcess();
		}
	}
	infoOutage.offset=0;
	return false;
}


void Outage_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

bool isOutage(void)
{
	if((GPIOA->IDR & GPIO_Pin_2)!=RESET)
		return false;
	return true;
}

