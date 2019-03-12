#include "includes.h"
#include "biqugui.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "stm32f10x_flash.h"

GUI_RECT menuRect[8]={
{12,	60,		108,	189},
{132,	60,		228,	189},
{252,	60,		348,	189},
{372,	60,		468,	189},
{12,	190,	108,	319},
{132,	190,	228,	319},
{252,	190,	348,	319},
{372,	190,	468,	319},
};
GUI_RECT menuRectPrint[13]={
{12,	60,		12+96 ,	155},
{132,	60,		132+96,	155},
{252,	60,		252+96,	155},
{372,	60,		372+96,	155},
{12,	190,	12+96 ,	285},
{132,	190,	132+96,	319},
{252,	190,	252+96,	319},
{372,	190,	372+96,	319},

{12,	156,	12+96 ,	189},
{132,	156,	132+96,	189},
{252,	156,	252+96,	189},
{372,	156,	372+96,	189},
{12,	286,	12+96 ,	319},
};
GUI_RECT menuRectOutage[2]={
{20,200,140,250},
{340,200,460,250},
};

u8 cur_view=MENU_MAIN;


typedef struct
{
	u32 	time;
	s16		offset;
	s16 	off_2;
	u8 		select;
	char 	*name;
	u8  	len;
	u8  	file;	
}FILE_SELECT;

FILE_SELECT infoLongFile={0,0,0,0,0,0,0};

void lnameDisp(void)
{
	u16 i=0;
	u16 sx=menuRect[infoLongFile.select].x0,
			sy=menuRect[infoLongFile.select].y0+100,
			ex=menuRect[infoLongFile.select].x1,
			ey=menuRect[infoLongFile.select].y0+124;
			
	if(infoLongFile.len>8)
	{
		if(OS_GetTime()-infoLongFile.time>5)
		{
			infoLongFile.time=OS_GetTime();			
			infoLongFile.offset++;
			
			GUI_SetRange(sx,sy,ex,ey);
			GUI_DispLenString(sx-infoLongFile.offset,sy,(void *)infoLongFile.name,1,infoLongFile.len);
			for(i=sy;i<=ey;i++)
			{
				GUI_DrawPixel(sx-infoLongFile.offset+(infoLongFile.len*12),i,BLACK);
			}

			if(infoLongFile.len*12-infoLongFile.offset<60)
			{
				infoLongFile.off_2++;
				GUI_DispLenString(96+sx-infoLongFile.off_2,sy,(void *)infoLongFile.name,1,infoLongFile.len);
				if(infoLongFile.off_2>=96)
				{
					infoLongFile.offset=0;
					infoLongFile.off_2=0;
				}
			}		
			GUI_CancelRange();
		}
	}
}

#define NUM_PER_PAGE 5
void nameDisp(char file, char i, u8 *name)
{
	u8 len,x_offset; 	
	
	len=my_strlen(name);	
	if(file)
		len-=6;
	if(len>8)
	{		
		len=8;
	}
	
	x_offset=((96-(len*12))>>1);	
	GUI_DispLenString(menuRect[i].x0+x_offset,menuRect[i].y0+100,name,1,len);			
}

void fillLongFile(u8 i)
{
	u8 num=infoMyfile.cur_page*NUM_PER_PAGE+i;
	memset(&infoLongFile,0,sizeof(FILE_SELECT));
	
	infoLongFile.select=i;
	infoLongFile.offset=0;
	infoLongFile.off_2=0;
	
	if(num<infoMyfile.F_num)
	{
		infoLongFile.name=infoMyfile.folder[num];
		infoLongFile.file=0;
		infoLongFile.len=my_strlen((void *)infoLongFile.name);
	}
	else if(num<infoMyfile.F_num+infoMyfile.f_num)
	{
		infoLongFile.name=infoMyfile.file[num-infoMyfile.F_num];
		infoLongFile.file=1;
		infoLongFile.len=my_strlen((void *)infoLongFile.name)-6;
	}
	
}

u16 unitConvert(u16 unit1,u16 unit2,u16 unit3,u16 cur_unit)
{
	if(cur_unit==unit1)		return unit2;
	if(cur_unit==unit2)		return unit3;
	return unit1;
}


void infoDisplay( u8 *warning)
{
	Show_warning(BLACK,RED,warning);
	err_flag=200;
}

bool storeCmd(char *cmd)
{
	if (infoCMD.count >= BUFSIZE)
	{	
		infoDisplay(textSelect(BUSY));
		return false;
	}
	strcpy(infoCMD.queue[infoCMD.index_w], cmd);
	infoCMD.index_w = (infoCMD.index_w + 1) % BUFSIZE;
	infoCMD.count++;
	return true;
}



void menuPicClear(void)
{
	GUI_FillRect(0,10,479,10+23,BLACK);
	GUI_FillRect(0,160,479,160+23,BLACK);
	GUI_FillRect(0,290,479,290+23,BLACK);
	GUI_FillRect(107,60,131,285,BLACK);
	GUI_FillRect(227,60,251,285,BLACK);
	GUI_FillRect(347,60,371,285,BLACK);
	if(0==infoUSART1_CMD.link_flag)
	{
		Show_warning(BLACK,RED,textSelect(NOTCONNECT));
	}
}
/*************************主界面***********************/
void Read_Display_black(u8 i)
{
	u16 x,y;
	u16 sx=menuRect[i].x0,
			sy=menuRect[i].y0,
			ex=sx+94,
			ey=sy+94;
	
	LCD_SetWindow(sx,sy,ex,ey);
	for(y=sy;y<=ey;y++)
	{	
		for(x=sx;x<=ex;x++)
		{
			LCD_WR_16BITS_DATA(BLACK);
		}
	}		
}
void mainMenuPic(void)
{
	menuPicClear();
	cur_view=MENU_MAIN;
	
	GUI_DispString(10,10,textSelect(READY),0);
	Read_Display_Photo(0,HEAT_ICON,0);
	Read_Display_Photo(1,MOVE_ICON,0);
	Read_Display_Photo(2,HOME_ICON,0);
	Read_Display_Photo(3,PRINT_ICON,0);
	Read_Display_Photo(4,EXTRUDE_ICON,0);
	Read_Display_Photo(5,FAN_ICON,0);
	Read_Display_Photo(6,SET_ICON,0);
	Read_Display_Photo(7,LEVELING_ICON,0);
}

void mainMenu(void)
{
	u8 key_num;
	u8 printToPrinting=0;
	mainMenuPic();
	while(1)
	{
		key_num=Get_Place(MENU_MAIN);
		
		switch(key_num)
		{
			case 1:heatMenu();	 mainMenuPic();   break;
			case 2:moveMenu();   mainMenuPic();break;
			case 3:homeMenu();   mainMenuPic();break;
			case 4:
				do
				{
					printToPrinting=printMenu();
					if(printToPrinting==0)
					{
						break;
					}
						printingMenu();
				}while(printToPrinting);
				mainMenuPic();
				break;
			case 5:extrudeMenu();   mainMenuPic();break;
			case 6:fanMenu();   		mainMenuPic();break;
			case 7:settingMenu();   mainMenuPic();break;
			case 8:
				storeCmd(CMD_AUTO_HOME);
				storeCmd(CMD_AUTO_LEVELING);
				break;
		}
		loopProcess();
	}
}



/*************************预热***********************/
void heatValueDisplay(void)
{
	if(infoHeat.type==NOZZLE)
	{
		GUI_DispString(220,90," E1",1);
		Show_num(232-12*3,120,infoHeat.cur_nozzle,3,RIGHT,1);
		Show_num(232+12,120,infoHeat.tag_nozzle,3,LEFT,1);
	}
	else
	{
		GUI_DispString(220,90,"Bed",1);
		Show_num(232-12*3,120,infoHeat.cur_bed,3,RIGHT,1);
		Show_num(232+12,120,infoHeat.tag_bed,3,LEFT,1);
	}
}



void heatMenuPic(void)
{
	menuPicClear();
	cur_view=MENU_HEAT;
	
	GUI_DispString(10,10,textSelect(HEAT),0);
	Read_Display_Photo(0,ADD_ICON,0);
	Read_Display_black(1);
	Read_Display_black(2);
	Read_Display_Photo(3,SUB_ICON,0);
	if(infoHeat.type==NOZZLE)
		Read_Display_Photo(4,NOZZLE_ICON,0);
	else
		Read_Display_Photo(4,BED_ICON,0);
	Read_Display_Photo(5,DEGREE_ICON_1+infoHeat.heat_unit/5,0);
	Read_Display_Photo(6,STOP_ICON,0);
	Read_Display_Photo(7,BACK_ICON,0);
	heatValueDisplay();
	GUI_DispChar(232,120,'/',0);
	infoHeatUpdate.get_time=100;						//tim3由之前的3s一次中断，改为1s一次中断，在中断中存储“M105”来查询温度
}
u16 limitValue(u16 min_value,u16 cur_value,u16 max_value)
{
	return (cur_value<min_value)?(min_value):((cur_value>max_value)?max_value:cur_value);
}
bool heatStoreCmd(u16 type,u16 sendvalue)
{
	char cmdHeat[20];

	sendvalue=limitValue(0,sendvalue,type);
	if(type==NOZZLE)
	{
		sprintf(cmdHeat,"M104 S%d\n",sendvalue);
	}
	else
	{
		sprintf(cmdHeat,"M140 S%d\n",sendvalue);
	}
	return storeCmd(cmdHeat);
}

void heatProcessKey(u8 key_num,u16 *display_tag)
{
	switch(key_num)
	{
		case 1:
				if((*display_tag+infoHeat.heat_unit<=infoHeat.type)&&heatStoreCmd(infoHeat.type,(*display_tag)+infoHeat.heat_unit)==true)
				{
					(*display_tag)+=infoHeat.heat_unit;
					if(infoHeat.type==NOZZLE&&infoFan.fan_speed!=0)
					{
						infoFan.fan_speed=0;
						fanStoreCmd(0);
					}
				}
			break;
		case 2:break;
		case 3:break;
		case 4:
				if((*display_tag-infoHeat.heat_unit>=0)&&heatStoreCmd(infoHeat.type,(*display_tag)-infoHeat.heat_unit)==true)
				{
					(*display_tag)-=infoHeat.heat_unit;
				}
			break;
		case 5:
			if(infoHeat.type==NOZZLE)
			{
				infoHeat.type=BED;
				Read_Display_Photo(4,BED_ICON,0);
			}
			else
			{
				infoHeat.type=NOZZLE;
				Read_Display_Photo(4,NOZZLE_ICON,0);
			}
			heatValueDisplay();
			break;
		case 6:
			infoHeat.heat_unit=unitConvert(1,5,10,infoHeat.heat_unit);
			Read_Display_Photo(5,DEGREE_ICON_1+infoHeat.heat_unit/5,0);
			break;
		case 7:
				if(heatStoreCmd(infoHeat.type,0)==true)
				{
					(*display_tag)=0;
					if(infoHeat.type==NOZZLE&&infoFan.fan_speed!=255)
					{
						infoFan.fan_speed=255;
						fanStoreCmd(255);
					}
				}			
			break;
		default:break;
	}
	return ;
}
void heatMenu(void)
{
	u8 key_num;
	
	heatMenuPic();
	while(1)
	{
		key_num=Get_Place(MENU_HEAT);
		if(key_num==8)
			break;
		
		if(infoHeat.type==NOZZLE)
		{
			Show_num(232-12*3,120,infoHeat.cur_nozzle,3,RIGHT,1);
			Show_num(232+12,120,infoHeat.tag_nozzle,3,LEFT,1);
		}
		else
		{
			Show_num(232-12*3,120,infoHeat.cur_bed,3,RIGHT,1);
			Show_num(232+12,120,infoHeat.tag_bed,3,LEFT,1);
		}
		if(key_num!=0)
		{
			if(infoHeat.type==NOZZLE)
				heatProcessKey(key_num,&infoHeat.tag_nozzle);
			else
				heatProcessKey(key_num,&infoHeat.tag_bed);
		}
		loopProcess();
	}
	infoHeatUpdate.get_time=300;			//退出预热界面后，改为3s查询一次温度
}






/***************************************************移动************************************************/

void moveMenuPic(void)
{
	menuPicClear();
	cur_view=MENU_MOVE;
	
	
	GUI_DispString(10,10,textSelect(MOVE),0);
	
	Read_Display_Photo(0,XADD_ICON,0);
	Read_Display_Photo(1,YADD_ICON,0);
	Read_Display_Photo(2,ZADD_ICON,0);
	Read_Display_Photo(3,MMM_01_ICON+infoMove.move_unit%8,0);	
	Read_Display_Photo(4,XSUB_ICON,0);
	Read_Display_Photo(5,YSUB_ICON,0);
	Read_Display_Photo(6,ZSUB_ICON,0);
	Read_Display_Photo(7,BACK_ICON,0);

}
void moveStoreCmd(char xyz, char change,u8 move_unit)
{
	char cmdMove[20];
	if(move_unit!=0)
		sprintf(cmdMove,"G1 %c%c%d\n",xyz,change,move_unit);
	else
		sprintf(cmdMove,"G1 %c%c0.1\n",xyz,change);

	storeCmd(cmdMove);
}
void moveMenu(void)
{
	u8 key_num;
	moveMenuPic();
	
	while(!storeCmd(CMD_XYZ_RELATIVE))
	{
		loopProcess();
	}	
	while(1)
	{
		key_num=Get_Place(MENU_MOVE);
		if(key_num==8)
			break;
		switch(key_num)
		{
			case 1:moveStoreCmd('X','+',infoMove.move_unit);break;
			case 2:moveStoreCmd('Y','+',infoMove.move_unit);break;
			case 3:moveStoreCmd('Z','+',infoMove.move_unit);break;
			case 4:
				infoMove.move_unit=unitConvert(0,1,10,infoMove.move_unit);
				Read_Display_Photo(3,MMM_01_ICON+infoMove.move_unit%8,0);	
				break;
			case 5:moveStoreCmd('X','-',infoMove.move_unit);break;
			case 6:moveStoreCmd('Y','-',infoMove.move_unit);break;
			case 7:moveStoreCmd('Z','-',infoMove.move_unit);break;
			default:break;
		}
		loopProcess();
	}
	while(!storeCmd(CMD_XYZ_ABSOLUTE))
	{
		loopProcess();
	}	
}

/****************************************归零*********************************************/
void homeMenuPic(void)
{
	menuPicClear();
	cur_view=MENU_HOME;
	
	
	GUI_DispString(10,10,textSelect(HOME),0);
	
	Read_Display_Photo(0,HOME_ICON,0);
	Read_Display_Photo(1,ZERO_X_ICON,0);
	Read_Display_Photo(2,ZERO_Y_ICON,0);
	Read_Display_Photo(3,ZERO_Z_ICON,0);
	Read_Display_black(4);
	Read_Display_black(5);
	Read_Display_black(6);
	
	Read_Display_Photo(7,BACK_ICON,0);
}

void homeMenu(void)
{
	u8 key_num;
	homeMenuPic();
	while(1)
	{
		key_num=Get_Place(MENU_HOME);
		if(key_num==8)
			break;
		switch(key_num)
		{
			case 1:storeCmd("G28\n");break;
			case 2:storeCmd("G1 X0 F3000\n");break;
			case 3:storeCmd("G1 Y0 F3000\n");break;
			case 4:storeCmd("G1 Z0 F4000\n");break;
			case 5:break;
			case 6:break;
			case 7:break;
		}
		loopProcess();
	}
}

/***************************************打印***********************************/
FATFS getFileFs;
u8 Getfile_fromSD(void)
{
												/* Work area (file system object) for logical drives */ 
	u8 timeout;
	u8 ret=0;
	FRESULT getFileRes;
	do
	{
		while(SD_Init())//检测不到SD卡
		{
			Delay(50);
			timeout++;
			if(timeout>10)
			{
				ret=1;		
				break;
			}
		}
		if(ret==1)
			break;
		getFileRes = f_mount(&getFileFs,"0:",1);
		if(getFileRes != FR_OK)
		{
			ret=2;
			break;
		}
	}while(0);
		
	return ret;
}
void clearInfoFile(void);


void printListPic(void)
{
	u8 i=0;
	cur_view=MENU_PRINT;
	
	
	fillLongFile(0);
	
	for(i=0;i<5&&(i+infoMyfile.cur_page*5<infoMyfile.F_num+infoMyfile.f_num);i++)
	{
		if((i+infoMyfile.cur_page*5<infoMyfile.F_num))
		{
			Read_Display_Photo(i,FOLDER_ICON,0);
			GUI_FillRect(menuRect[i].x0-12,menuRect[i].y0+95,menuRect[i].x0-12+120,menuRect[i].y1,BLACK);
			nameDisp(0,i,(void *)infoMyfile.folder[i+infoMyfile.cur_page*NUM_PER_PAGE]);
			infoPrint.back_icon[i]=FOLDER_ICON;
		}
		else
		{
			Read_Display_Photo(i,FILE_ICON,0);
			GUI_FillRect(menuRect[i].x0-12,menuRect[i].y0+95,menuRect[i].x0-12+120,menuRect[i].y1,BLACK);
			nameDisp(1,i,(void *)infoMyfile.file[i+infoMyfile.cur_page*NUM_PER_PAGE-infoMyfile.F_num]);
			infoPrint.back_icon[i]=FILE_ICON;
		}
	}
	for(;i<5;i++)
	{
		GUI_FillRect(menuRect[i].x0-12,menuRect[i].y0,menuRect[i].x0-12+120,menuRect[i].y1,BLACK);
		infoPrint.back_icon[i]=0;
	}
}
u8 printMenu(void)
{
	u8 key_num;
	u8 ret;
	char update=1;
	
	LCD_Clear(BLACK);
	GUI_DispString(150,160,textSelect(LOADING),0);
	

	infoMyfile.title[0]="0:";
	ret=Getfile_fromSD();
	if(ret!=0)
	{
		switch(ret)
		{
			case 1:
			GUI_DispString(150,110,textSelect(NOCARD),0);
			break;
			case 2:
			GUI_DispString(150,110,textSelect(FILESYS_FAILED),0);
			break;
			default:break;
		}
		Delay_ms(1000);
		return 0;
	}	
	if(outageMenu())
	{
		return 1;
	}
	
	Scan_Files();
	menuPicClear();
	GUI_DispString(10,10,textSelect(PRINT),0);
	Read_Display_Photo(5,LAST_ICON,0);
	Read_Display_Photo(6,NEXT_ICON,0);
	Read_Display_Photo(7,BACK_ICON,0);
	printListPic();

	while(1)
	{
		key_num=Get_Place(MENU_PRINT);
		
		if(key_num!=0) 
		{
			if(key_num==8)
			{
				infoMyfile.cur_page=0;
				if(infoMyfile.cur_nesting==0)
				{
					clearInfoFile();
	//					cur_menu=mainMenu;
					return 0;
				}
				else
				{
					Read_Display_Photo(7,BACK_ICON,0);
					free(infoMyfile.title[infoMyfile.cur_nesting]);
					infoMyfile.title[infoMyfile.cur_nesting--]=NULL;					
					strcpy(infoPrint.file,infoMyfile.title[infoMyfile.cur_nesting]);
					Scan_Files();
				
					update=1;
				}
			}
			else if(key_num<=5)
			{
				key_num--;
//				printf("\r\nfile:%d\r\n",(key-MENU_PRINT*8)+nowp*5-F_num-1);
				if(key_num+infoMyfile.cur_page*5<infoMyfile.F_num+infoMyfile.f_num)
				{					
					u16 i=strlen(infoPrint.file);
					if((key_num)+infoMyfile.cur_page*5<infoMyfile.F_num )					//文件夹
					{
 						i+=strlen((char *)infoMyfile.folder[(key_num)+infoMyfile.cur_page*NUM_PER_PAGE])+1;
						if(i<_MAX_LFN)
						{
							infoMyfile.title[++infoMyfile.cur_nesting]=malloc(i+1);
							
							if(infoMyfile.title[infoMyfile.cur_nesting]!=NULL)
							{							
								strcat(infoPrint.file,"/");						
								strcat(infoPrint.file,infoMyfile.folder[(key_num)+infoMyfile.cur_page*5]);

								strcpy(infoMyfile.title[infoMyfile.cur_nesting],infoPrint.file);					
								Scan_Files();
			
								update=1;
								infoMyfile.cur_page=0;
							}
							else
							{
								infoMyfile.cur_nesting--;
							}
						}
					}
					else													//gcode文件
					{						
						if(1==infoUSART1_CMD.link_flag)
						{
							i+=strlen((char *)infoMyfile.file[(key_num)+infoMyfile.cur_page*NUM_PER_PAGE-infoMyfile.F_num])+1;
							if(i<_MAX_LFN)
							{
								strcat(infoPrint.file,"/");
								strcat(infoPrint.file,infoMyfile.file[(key_num)+infoMyfile.cur_page*5-infoMyfile.F_num]);
								memcpy(infoPrint.dispfile,infoMyfile.file[(key_num)+infoMyfile.cur_page*5-infoMyfile.F_num],limitValue(0,strlen(infoMyfile.file[(key_num)+infoMyfile.cur_page*5-infoMyfile.F_num])+1,40));
								break;
							}	
						}
					}
				}
			}
			else if(key_num==6)
			{				
				if(infoMyfile.cur_page>0)
				{
					infoMyfile.cur_page--;
					update=1;
				}
			}
			else if(key_num==7)
			{
				if(infoMyfile.cur_page+1<(infoMyfile.F_num+infoMyfile.f_num+4)/5)
				{
					infoMyfile.cur_page++;
					update=1;
				}				
			}	
			else
			{
				if(infoLongFile.name!=NULL)
				{
					GUI_FillRect(menuRectPrint[infoLongFile.select].x0,menuRectPrint[infoLongFile.select].y1,menuRectPrint[infoLongFile.select].x1,menuRectPrint[infoLongFile.select].y1+30,BLACK);
					nameDisp(infoLongFile.file,infoLongFile.select,(void *)infoLongFile.name);
				}
				fillLongFile(key_num-9);
			}
		}
		if(1==update)
		{			
			update=0;
			printListPic();
		}
		loopProcess();
		lnameDisp();
	}
	return 1;
}
void printingMenu(void)
{
		Print_SdFile(infoPrint.file,infoOutage.offset);		
		strcpy(infoPrint.file,infoMyfile.title[infoMyfile.cur_nesting]);
}

/**************************************打印中*******************************/
void printingTransmissionPic(void)
{
	menuPicClear();
	cur_view=MENU_SPEED;
	GUI_DispString(10,10, textSelect(SPEED_ICON),0);
	
	Read_Display_Photo(0,ADD_ICON,0);
	Read_Display_black(1);
	Read_Display_black(2);
	Read_Display_Photo(3,SUB_ICON,0);	
	Read_Display_Photo(4,EXTRUDE_ICON,!infoPrinting.speed_type);
	Read_Display_Photo(5,MOVE_ICON,infoPrinting.speed_type);
	Read_Display_Photo(6,EMM_ICON_1+infoPrinting.speed_unit/5,0);
	Read_Display_Photo(7,BACK_ICON,0);
}
void transmissionStoreCmd(char *transcmd,u16 transspeed)
{
	char cmdTrans[20];
	sprintf(cmdTrans,"%s S%d\n",transcmd,transspeed);
	storeCmd(cmdTrans);
}

void transmissionProcessKey(u8 key_num)
{
	switch(key_num)
	{
		case 1:
			if(infoPrinting.speed_type==0)
			{
				infoPrinting.e_speed=limitValue(10,infoPrinting.e_speed+infoPrinting.speed_unit,999);
				transmissionStoreCmd("M221",infoPrinting.e_speed);
			}
			else
			{
				infoPrinting.xyz_speed=limitValue(10,infoPrinting.xyz_speed+infoPrinting.speed_unit,999);
				transmissionStoreCmd("M220",infoPrinting.xyz_speed);
			}
			break;
		case 4:
			if(infoPrinting.speed_type==0)
			{
				infoPrinting.e_speed=limitValue(10,infoPrinting.e_speed-infoPrinting.speed_unit,999);
				transmissionStoreCmd("M221",infoPrinting.e_speed);
			}
			else
			{
				infoPrinting.xyz_speed=limitValue(10,infoPrinting.xyz_speed-infoPrinting.speed_unit,999);
				transmissionStoreCmd("M220",infoPrinting.xyz_speed);
			}
			break;
		case 5:
			if(infoPrinting.speed_type!=0)
			{
				infoPrinting.speed_type=0;
				Read_Display_Photo(4,EXTRUDE_ICON,1);
				Read_Display_Photo(5,MOVE_ICON,0);
			}
			break;
		case 6:
			if(infoPrinting.speed_type==0)
			{
				infoPrinting.speed_type=1;
				Read_Display_Photo(4,EXTRUDE_ICON,0);
				Read_Display_Photo(5,MOVE_ICON,1);
			}
			break;
		case 7:
		infoPrinting.speed_unit=unitConvert(1,5,10,infoPrinting.speed_unit);
		Read_Display_Photo(6,EMM_ICON_1+(infoPrinting.speed_unit)/5,0);
		break;
		default:break;
	}
		
}


/****************************************挤出**********************************/


void extrudeMenuPic(void)
{
	menuPicClear();
	cur_view=MENU_EXTRUDER;
	
	GUI_DispString(10,10, textSelect(EXTRUDE),0);
	Read_Display_Photo(0,EADD_ICON,0);
	Read_Display_black(1);
	Read_Display_black(2);
	GUI_DispString(180,100, "E1(mm):",0);
	Show_num(180+7*12,100,infoExtrude.len_totel,8,LEFT,1);
	Read_Display_Photo(3,ESUB_ICON,0);
	Read_Display_Photo(4,NOZZLE_ICON,0);
	Read_Display_Photo(5,SLOW_ICON+infoExtrude.speed_unit/EXTRUDE_NORMAL_SPEED,0);
	Read_Display_Photo(6,EMM_ICON_1+infoExtrude.move_unit/5,0);
	Read_Display_Photo(7,BACK_ICON,0);

}
bool extrudeStoreCmd(char change,u8 extrude_unit,u16 extrude_speed)
{
	char cmdExtrude[20];
	if(1==infoUSART1_CMD.link_flag)
	{
#ifdef PREVENT_COLD_EXTRUSION
		if(infoHeat.cur_nozzle>=EXTRUDE_MINTEMP)
#endif
		{
			sprintf(cmdExtrude,"G1 E%c%d F%d\n",change,extrude_unit,extrude_speed);			
			return storeCmd(cmdExtrude);
		}	
#ifdef PREVENT_COLD_EXTRUSION
		else
		{
			infoDisplay(textSelect(MIN_TEMP));
		}
#endif
	}
	return false;
}

void extrudeMenu(void)
{
	u8 key_num;
	extrudeMenuPic();
	storeCmd(CMD_E_RELATIVE);
	while(1)
	{
		key_num=Get_Place(MENU_EXTRUDER);
		if(key_num==8)
			break;
		switch(key_num)
		{
			case 1:
				if(extrudeStoreCmd('+',infoExtrude.move_unit,infoExtrude.speed_unit)==true)
				{					
					infoExtrude.len_totel+=infoExtrude.move_unit;
				}
				break;
			case 2:break;
			case 3:break;
			case 4:
				if(extrudeStoreCmd('-',infoExtrude.move_unit,infoExtrude.speed_unit)==true)
				{					
					infoExtrude.len_totel-=infoExtrude.move_unit;
				}
				break;
			case 5:break;
			case 6:
				infoExtrude.speed_unit=unitConvert(60,600,1200,infoExtrude.speed_unit);
				Read_Display_Photo(5,SLOW_ICON+infoExtrude.speed_unit/600,0);
				break;
			case 7:
				infoExtrude.move_unit=unitConvert(1,5,10,infoExtrude.move_unit);
				Read_Display_Photo(6,EMM_ICON_1+(infoExtrude.move_unit)/5,0);

				break;
		}
		Show_num(180+7*12,100,infoExtrude.len_totel,8,LEFT,1);
		loopProcess();
	}
	storeCmd(CMD_E_ABSOLUTE);
}
/****************************************风扇**********************************/


void fanMenuPic(void)
{
	menuPicClear();
	cur_view=MENU_FAN;
	
	
	GUI_DispString(10,10, textSelect(FAN),0);
	Read_Display_Photo(0,ADD_ICON,0);
	Read_Display_black(1);
	Read_Display_black(2);
	GUI_DispString(210,85, "FAN",0);
	Show_num(210,110,infoFan.fan_speed,3,LEFT,1);
	Read_Display_Photo(3,SUB_ICON,0);
	Read_Display_Photo(4,FULL_ICON,0);
	Read_Display_Photo(5,HALF_ICON,0);
	Read_Display_Photo(6,STOP_ICON,0);
	Read_Display_Photo(7,BACK_ICON,0);
}

bool fanStoreCmd(u8 fan_value)
{
	char cmdFan[20];
	if(1==infoUSART1_CMD.link_flag)
	{
		sprintf(cmdFan,"M106 S%d\n",fan_value);
		return storeCmd(cmdFan);
	}
	return false;
}
void fanProcessKey(u8 key_num)
{
	u8 fanSpeed=infoFan.fan_speed;
	switch(key_num)
	{
		case 1:
			fanSpeed=limitValue(0,infoFan.fan_speed+1,255);
			break;
		case 2:break;
		case 3:break;
		case 4:
			fanSpeed=limitValue(0,infoFan.fan_speed-1,255);
			break;
		case 5:
			fanSpeed=255;
			break;
		case 6:
			fanSpeed=127;
			break;
		case 7:
			fanSpeed=0;
			break;
	}
	if(fanStoreCmd(fanSpeed)==true)
	{
		infoFan.fan_speed=fanSpeed;
	}
}
void fanMenu(void)
{
	u8 key_num;
	fanMenuPic();
	while(1)
	{
		key_num=Get_Place(MENU_FAN);
		if(key_num==8)
			break;
		if(key_num!=0)
		fanProcessKey(key_num);
		loopProcess();
		Show_num(210,110,infoFan.fan_speed,3,LEFT,1);
	}
}

void disConnect(void)
{
	LCD_Clear(BLACK);
	GUI_DispString(100,130,"Now,you can control the",0);
	GUI_DispString(100,160,"printer by your computer",0);
	
	
	GPIO_DeInit(GPIOA);
	
	while(!press);
	while(press);
		
	W25QXX_Init();
	USART1_Config(115200);
	infoUSART1_CMD.wait=0;
	
}
/****************************************设置**********************************/
void infoMenu(void)
{
	LCD_Clear(BLACK);
	GUI_DispString(50,130,"Board   : BIQU_TFT35 V1.2",0);
	GUI_DispString(50,160,"Firmware: BIQU_TFT35_V1.2.8",0);
	while(!press) loopProcess();
	while(press)loopProcess();
}
#define LANGUE_ADDR   (0x0803F000-0x800)	
void settingMenuPic(void)
{
	menuPicClear();
	cur_view=MENU_SETTING;
	
	
	GUI_DispString(10,10, textSelect(SETTING),0);
	
	Read_Display_Photo(0,CLOSE_ICON,0);
	Read_Display_Photo(1,LANGUE_ICON,0);
	Read_Display_Photo(2,ADJUST_ICON,0);
	Read_Display_Photo(3,ABOUT_ICON,0);
	Read_Display_Photo(4,DISCONNECT_ICON,0);
	Read_Display_Photo(5,RESET_ICON,0);
	Read_Display_black(6);
	
	Read_Display_Photo(7,BACK_ICON,0);
	
	GUI_DispString(144,290,(u8 *)((infoSetting.baudrate==250000)?"250000":"115200"),1);
}
void settingMenu(void)
{
	u8  key_num;
	u8  now_language=infoSetting.language;
	u32 now_baudrate=infoSetting.baudrate;
	settingMenuPic();
	while(1)
	{
		key_num=Get_Place(MENU_SETTING);
		if(key_num==8)
			break;
		switch(key_num)
		{
			case 1:
				storeCmd(CMD_CLOSE_ST);
				break;
			case 2:
				infoSetting.language = (infoSetting.language + 1) % TOTEL_LANGUAGE;
				settingMenuPic();
				break;
			case 3:
				Three_adjust();settingMenuPic();break;
			case 4:
				infoMenu();settingMenuPic();break;
			case 5:
				disConnect();settingMenuPic();	break;
			case 6:
				if(infoSetting.baudrate==115200)
					infoSetting.baudrate=250000;
				else 
					infoSetting.baudrate=115200;
				GUI_DispString(144,290,(u8 *)((infoSetting.baudrate==250000)?"250000":"115200"),1);
				
				break;
			case 7:break;
		}
		loopProcess();
	}
	if(now_language!=infoSetting.language || now_baudrate != infoSetting.baudrate)
	{
		FLASH_Unlock();
		FLASH_ErasePage(LANGUE_ADDR);
		FLASH_ProgramHalfWord(LANGUE_ADDR,(u16)infoSetting.language);
		FLASH_ProgramWord(LANGUE_ADDR+4,infoSetting.baudrate);
		USART1_Config(infoSetting.baudrate);	
		FLASH_Lock();
	}
}






