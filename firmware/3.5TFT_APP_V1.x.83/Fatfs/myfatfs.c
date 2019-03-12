#include "includes.h"
#include "myfatfs.h"
#include "biqugui.h"

void clearInfoFile(void)
{	
	u8 i=0;
	for(i=0;i<infoMyfile.F_num;i++)
	{
		free(infoMyfile.folder[i]);
		infoMyfile.folder[i]=NULL;
	}
	for(i=0;i<infoMyfile.f_num;i++)
	{
		free(infoMyfile.file[i]);
		infoMyfile.file[i]=NULL;
	}
	infoMyfile.F_num=0;
	infoMyfile.f_num=0;
}

void Scan_Files(void)
{
	FILINFO scanInfo;
	DIR dir;
	char *fn;

	u8 len=0;

	clearInfoFile();
//	printf("title:%s\r\n",infoMyfile.title[infoMyfile.cur_nesting]);
	if(f_opendir(&dir,infoMyfile.title[infoMyfile.cur_nesting])==FR_OK)
	{
		for(;;)
		{
			if(f_readdir(&dir,&scanInfo)!=FR_OK||scanInfo.fname[0]==0)break;
			fn=scanInfo.fname;
			
			len=strlen(fn)+1;
			if((scanInfo.fattrib&AM_DIR)&&!(scanInfo.fattrib&AM_HID) && infoMyfile.F_num<FOLDER_NUM)
			{
				infoMyfile.folder[infoMyfile.F_num]=malloc(len);
				if(infoMyfile.folder[infoMyfile.F_num]==NULL)
						break;
				memcpy(infoMyfile.folder[infoMyfile.F_num++],fn,len);
//				printf("folder:%s\r\n",infoMyfile.folder[infoMyfile.F_num-1]);
			}
			else if(!(scanInfo.fattrib&AM_HID) && infoMyfile.f_num<FILE_NUM)
			{
				if(strstr(fn,".gcode")!=NULL)
				{
					infoMyfile.file[infoMyfile.f_num]=malloc(len);
					if(infoMyfile.file[infoMyfile.f_num]==NULL)
							break;
					memcpy(infoMyfile.file[infoMyfile.f_num++],fn,len);
//				  printf("file:%s\r\n",infoMyfile.file[infoMyfile.f_num-1]);
				}
			}
		}
	}
	return ;
}

float babyUnit[3]={1,0.1,0.01};
u8 		babyIcon[3]={MMM_1_ICON,MMM_01_ICON,RESERVE_2_ICON};

void babyStepMenuPic(void)
{	
	bool sel[3]={0,0,0};
	storeCmd("M851\n");
	menuPicClear();
	cur_view=MENU_BABY;
	
	sel[infoPrinting.baby_unit]=1;
	GUI_DispString(10,10,textSelect(RESERVE_1),0);
	Read_Display_Photo(0,ADD_ICON,0);
	Read_Display_black(1);
	Read_Display_black(2);
	Show_Float(192,100,infoPrinting.baby_step,2,2);
	Read_Display_Photo(3,SUB_ICON,0);
	Read_Display_Photo(4,babyIcon[0],sel[0]);
	Read_Display_Photo(5,babyIcon[1],sel[1]);
	Read_Display_Photo(6,babyIcon[2],sel[2]);
	Read_Display_Photo(7,BACK_ICON,0);

}

void  babyStepStoreCmd(char *cmd,float value)
{
	char cmdBaby[20];
	sprintf(cmdBaby,"%s Z%.2f\n",cmd,value);
	storeCmd(cmdBaby);
}
void babyStepProcessKey(u8 key_num)
{
	switch(key_num)
	{
		case 1:;
			if(infoPrinting.baby_step+babyUnit[infoPrinting.baby_unit]<99.99)
			{
				infoPrinting.baby_step+=babyUnit[infoPrinting.baby_unit];
				babyStepStoreCmd("M851",infoPrinting.baby_step);
			}
			break;
		case 4:
			if(infoPrinting.baby_step-babyUnit[infoPrinting.baby_unit]>-99.99)
			{
				infoPrinting.baby_step-=babyUnit[infoPrinting.baby_unit];
				babyStepStoreCmd("M851",infoPrinting.baby_step);
			}
			break;
		case 5:;
		case 6:;
		case 7:;
			if(infoPrinting.baby_unit!=key_num-5)
			{
				Read_Display_Photo(4+infoPrinting.baby_unit,babyIcon[infoPrinting.baby_unit],0);
				infoPrinting.baby_unit=key_num-5;
				Read_Display_Photo(key_num-1,babyIcon[infoPrinting.baby_unit],1);
			}
			break;
		default:break;
	}
		
}


/***********************************************************************************/
void printingMenuPic(u8 process)
{

	cur_view=MENU_PRINTING;
	
	menuPicClear();
	GUI_DispString(10,10,textSelect(PRINTING),0);

	GUI_DispString(10+(my_strlen(textSelect(PRINTING)))*12,10,(void *)infoPrint.dispfile,0);			
	
	Read_Display_Photo(0,PAUSE_ICON+infoPrinting.pause,0);
	Read_Display_black(1);
	Read_Display_black(2);
	Read_Display_Photo(3,STOP_ICON,0);
	Read_Display_Photo(4,HEAT_ICON,0);
	Read_Display_Photo(5,FAN_ICON ,0);
	Read_Display_Photo(6,SPEED_ICON ,0);
	Read_Display_Photo(7,RESERVE_1_ICON,0);
	
	
	GUI_DrawRect(126,80,354,120,GRAY);
	GUI_FillRect(127,81,127+process*2.27,119,BLUE);
	GUI_FillRect(127+process*2.27,81,354,119,GRAY);
	Show_num(216,88,process,3,1,0);
	GUI_DispChar(216+36,88,'%',0);
	
	GUI_DispString(126,160,"T:",0);
	GUI_DispChar(150+24,160,':',0);
	GUI_DispChar(150+60,160,':',0);
	Show_Clock(150,160,infoPrinting.time);
	
	GUI_DispString(126,130,"E:",0);
	Show_num(126+12*2,130,infoHeat.cur_nozzle,3,RIGHT,0);
	GUI_DispChar(126+12*5,130,'/',0);
	Show_num(126+12*6,130,infoHeat.tag_nozzle,3,0,0);

	GUI_DispString(246,130,"B:",0);
	Show_num(246+12*2,130,infoHeat.cur_bed,3,RIGHT,0);
	GUI_DispChar(246+12*5,130,'/',0);
	Show_num(246+12*6,130,infoHeat.tag_bed,3,0,0);
}
void Send_CMD(void);
bool storeCmd(char *str);

FIL printFile;
UINT printbr;

u8 divFunction(u32 num,u32 den)
{
	if(den==0)
		return 100;
	return (num/den);
}

void Print_SdFile(const TCHAR* path,u32 lseek)
{
	u8 	key_num;
	u8 	process=0;
	u32 old_time=0; 
	
	float dispbed=0,dispnozzle=0;
	float	dispbaby=0;	
	u8   outage_start=0;	
	
	bool			pause_flag=false;
	bool 			sd_comment_mode = false;
  bool 			sd_comment_space= true;
	char 			sd_char;
	u8  			sd_count=0;
	
	FIL 			outagefp;
	
	if(f_open(&outagefp,OUTAGE_FILE,FA_OPEN_ALWAYS|FA_WRITE)!=FR_OK || f_open(&printFile, path, FA_OPEN_EXISTING | FA_READ)!=FR_OK)
	{
		GUI_DispString(100,100,textSelect(FILE_OPEN_FAILED),0);
		Delay_ms(1500);
		return ;
	}
	strcpy(infoOutage.path,path);
	f_lseek(&printFile,lseek);
	
	printingMenuPic(divFunction(printFile.fptr,f_size(&printFile)/100));
	
	infoPrinting.ok=1;
	infoPrinting.time=0;
	transmissionStoreCmd("M220",infoPrinting.xyz_speed);
	transmissionStoreCmd("M221",infoPrinting.e_speed);
	while(1)
	{
		key_num=Get_Place(cur_view);

		switch(cur_view)
		{
			case MENU_PRINTING:
			{
				if(process!=divFunction(printFile.fptr,f_size(&printFile)/100))
				{
					process=divFunction(printFile.fptr,f_size(&printFile)/100);
					GUI_FillRect(127,81,127+process*2.27,119,BLUE);
					GUI_FillRect(127+process*2.27,81,354,119,GRAY);
					Show_num(216,88,process,3,1,0);
					GUI_DispChar(216+36,88,'%',0);
				}
				if(dispnozzle!=infoHeat.cur_nozzle+infoHeat.tag_nozzle)
				{
					dispnozzle=infoHeat.cur_nozzle+infoHeat.tag_nozzle;
					Show_num(126+12*2,130,infoHeat.cur_nozzle,3,RIGHT,1);
					Show_num(126+12*6,130,infoHeat.tag_nozzle,3,LEFT,1);
				}
				if(dispbed!=infoHeat.cur_bed+infoHeat.tag_bed)
				{
					dispbed=infoHeat.cur_bed+infoHeat.tag_bed;
					Show_num(246+12*2,130,infoHeat.cur_bed,3,RIGHT,1);
					Show_num(246+12*6,130,infoHeat.tag_bed,3,LEFT,1);
				}		
				if(old_time!=infoPrinting.time)
				{					
					Show_Clock(150,160,infoPrinting.time);
					old_time=infoPrinting.time;
				}
				if(key_num!=0)
				{
					if(key_num==4)
					{
						infoPrinting.ok=0;
						infoPrinting.time=0;
						goto file_end;
					}				
					switch(key_num)
					{
						case 1:
							if(infoCMD.count<BUFSIZE-3)
							{
								if(infoPrinting.ok==true)
								{
									infoPrinting.pause=!infoPrinting.pause;
									if(infoPrinting.pause)
									{
										if(!infoPrinting.wait)
										{
											pause_flag=true;
											storeCmd(CMD_XYZ_RELATIVE);
											storeCmd("G1 E-5\n");
											storeCmd("G1 Z20\n");
										}									
									}
									else
									{
										if(pause_flag)
										{
											pause_flag=false;
											storeCmd("G1 E5.5\n");
											storeCmd("G1 Z-20\n");
											storeCmd(CMD_XYZ_ABSOLUTE);
										}
									}
								}
							}
							else 
							{
								infoDisplay(textSelect(BUSY));
							}
							Read_Display_Photo(0,PAUSE_ICON+infoPrinting.pause,0);
							break;
						case 5:
							heatMenuPic();
							break;
						case 6:
							fanMenuPic();
							break;
						case 7:
							printingTransmissionPic();
							break;
						case 8:
							babyStepMenuPic();
							break;
					}
				}
			}
			break;
			case MENU_HEAT:
			{
				if(infoHeat.type==NOZZLE)
				{
					if(dispnozzle!=infoHeat.cur_nozzle+infoHeat.tag_nozzle)
					{
						dispnozzle=infoHeat.cur_nozzle+infoHeat.tag_nozzle;
						Show_num(232-12*3,120,infoHeat.cur_nozzle,3,RIGHT,1);
						Show_num(232+12,120,infoHeat.tag_nozzle,3,LEFT,1);
					}
				}
				else
				{
					if(dispbed!=infoHeat.cur_bed+infoHeat.tag_bed)
					{
						dispbed=infoHeat.cur_bed+infoHeat.tag_bed;
						Show_num(232-12*3,120,infoHeat.cur_bed,3,RIGHT,1);
						Show_num(232+12,120,infoHeat.tag_bed,3,LEFT,1);
					}
				}
				if(key_num!=0)
				{
					if(key_num==8)
					{
						printingMenuPic(process);
						infoHeatUpdate.get_time=300;
					}
					else
					{
						if(infoHeat.type==NOZZLE)
							heatProcessKey(key_num,&infoHeat.tag_nozzle);
						else
							heatProcessKey(key_num,&infoHeat.tag_bed);
					}
				}
			}
			break;
			case MENU_FAN:
			{
				Show_num(210,110,infoFan.fan_speed,3,LEFT,1);
				if(key_num!=0)
				{
					if(key_num==8)
					{
						printingMenuPic(process);
					}
					else
					{
						fanProcessKey(key_num);
					}
				}
			}
			break;
			case MENU_SPEED:
			{
				if(infoPrinting.speed_type==0)
				{
					Show_num(210,100,infoPrinting.e_speed,3,RIGHT,1);
				}
				else
				{
					Show_num(210,100,infoPrinting.xyz_speed,3,RIGHT,1);
				}
				if(key_num!=0)
				{
					if(key_num==8)
					{
						printingMenuPic(process);
					}
					else
					{
						transmissionProcessKey(key_num);
					}
				}
			}
			break;
			case MENU_BABY:
			{
				if(dispbaby!=infoPrinting.baby_step)
				{
					dispbaby=infoPrinting.baby_step;
					Show_Float(192,100,infoPrinting.baby_step,2,2);
				}
				if(key_num!=0)
				{
					if(key_num==8)
					{
						printingMenuPic(process);
					}
					else
					{
						babyStepProcessKey(key_num);
					}
				}
			}
			break;
		}
		
		if(lseek!=0 && outage_start<outageQueueCount())
		{		
			if(infoCMD.count==0&&!infoPrinting.wait)
			{
				strcpy(infoCMD.queue[infoCMD.index_w], outageQueueP(outage_start));
				infoCMD.index_w = (infoCMD.index_w + 1) % BUFSIZE;
				infoCMD.count++;
				outage_start++;
			}
		}
		else	if (infoCMD.count==0	&& !f_eof(&printFile) 	&&	!infoPrinting.pause &&!infoPrinting.wait) 
		{
			for(;;)
			{
				if(f_read(&printFile,&sd_char,1,&printbr)==FR_OK)
				{
					if ( sd_char == '\n' ) 
					{
						sd_comment_mode = false; //for new command
						sd_comment_space= true;
						if(sd_count!=0)
						{
							infoCMD.queue[infoCMD.index_w][sd_count++] = '\n'; 
							infoCMD.queue[infoCMD.index_w][sd_count] = 0; //terminate string
							sd_count = 0; //clear buffer
						 
							infoCMD.index_w = (infoCMD.index_w + 1) % BUFSIZE;
							infoCMD.count++;
							
						}
						break;
					}
					else if (sd_count >= MAX_CMD_SIZE - 2) {	}
					else 
					{
						if (sd_char == ';') sd_comment_mode = true;
						else if(!sd_comment_mode)
						{
							if(sd_comment_space&&(sd_char== 'G'||sd_char == 'M'))	sd_comment_space=false;
							if (!sd_comment_space && sd_char != '\r') infoCMD.queue[infoCMD.index_w][sd_count++] = sd_char;
						}
					}
				}
				else
				{
					break;
				}
			}
		}
		if((f_eof(&printFile))&&(infoPrinting.ok==true))
		{
			infoPrinting.ok=false;
			
			f_unlink(OUTAGE_FILE);
		}
	
		loopProcess();
		if(outage_save==0x03)
		{
			outage_save=0;
			infoOutage.offset=printFile.fptr;
			f_lseek(&outagefp,0);
			f_write(&outagefp,&infoOutage,sizeof(infoOutage),&printbr);
			f_sync(&outagefp);
		}
	}
file_end:
	if(printFile.fptr<f_size(&printFile))
	{
		storeCmd("M104 S0\n");
		storeCmd("M140 S0\n");
		storeCmd(CMD_AUTO_HOME);
	}
	f_close(&printFile);	
	f_close(&outagefp);	
	f_unlink(OUTAGE_FILE);
	
	infoPrinting.pause=0; 
	
	infoPrinting.wait=0;		
	infoHeatUpdate.get_time=300;
	
	return ;
}

