#include "Printing.h"
#include "includes.h"

//1个title(标题), ITEM_PER_PAGE个item(图标+标签) 
MENUITEMS printingItems = {
//  title
    LABEL_BACKGROUND,
//   icon                       label
   {{ICON_PAUSE,                LABEL_PAUSE},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_STOP,                 LABEL_STOP},
    {ICON_HEAT,                 LABEL_HEAT},
    {ICON_FAN,                  LABEL_FAN},
    {ICON_PERCENTAGE,           LABEL_PERCENTAGE},
    {ICON_BABYSTEP,             LABEL_BABYSTEP},}
};

const ITEM itemIsPause[2] = {
//   icon                       label
    {ICON_PAUSE,                LABEL_PAUSE},
    {ICON_RESUME,               LABEL_RESUME},
};


static PRINTING infoPrinting;


//是否正在打印
bool isPrinting(void)
{
	return infoPrinting.printing;
}

//是否处于暂停状态
bool isPause(void)
{
	return infoPrinting.pause;
}

//更新打印耗时
void setPrintingTime(u32 RTtime)
{
	if(RTtime%100 == 0)
	{
		if(isPrinting() && !isPause())
		{
			infoPrinting.time++;      
		}
	}
}
//设置打印文件的总大小
void setPrintSize(u32 size)
{
    infoPrinting.size = size;
}
//设置当前已打印的大小
void setPrintCur(u32 cur)
{
    infoPrinting.cur = cur;
}

u8 getPrintProgress(void)
{
    return infoPrinting.progress;
}
u32 getPrintTime(void)
{
    return infoPrinting.time;
}


void startGcodeExecute(void)
{    
//    if(isResumePowerFailed() == false)
//    {
//        mustStoreCmd("G28 X0 Y0\n");
//        mustStoreCmd("G92 E0\n");
//    }
}

void endGcodeExecute(void)
{
    mustStoreCmd("G90\n");
    mustStoreCmd("G92 E0\n");
    mustStoreCmd("M104 S0\n");  
    mustStoreCmd("M140 S0\n");
    mustStoreCmd("M106 S0\n");
    mustStoreCmd("M18\n");
}



void menuBeforePrinting(void)
{
    if(f_open(&infoPrinting.file,infoFile.title, FA_OPEN_EXISTING | FA_READ)!=FR_OK)
    {		
        infoMenu.cur--;		
        return ;
    }
    if( powerFailedCreate(infoFile.title)==false)
    {
        //reminderMessage("无法断电续打");
    }	
    powerFailedlSeek(&infoPrinting.file);
    //  initPrintingSeek(infoPrinting.file.fptr);

    infoPrinting.size  = f_size(&infoPrinting.file);
    infoPrinting.cur   = infoPrinting.file.fptr;
    infoPrinting.printing = 1;
    
    startGcodeExecute();
    infoMenu.menu[infoMenu.cur] = menuPrinting;
}


void resumeToPause(bool is_pause)
{
    printingItems.items[KEY_ICON_0] = itemIsPause[is_pause];
    menuDrawItem(&itemIsPause[is_pause],0);
}

bool setPrintPause(bool is_pause)
{
	if(!isPrinting())                       return false;
	if(infoPrinting.pause == is_pause)      return false;
	
    while(infoCmd.count != 0) {loopProcess();}

    bool isCoorRelative = xyzGetRelative();
    bool isExtrudeRelative = eGetRelative();
        
    infoPrinting.pause = is_pause;    
    if(infoPrinting.pause)
    {             
        if( isCoorRelative == false || isExtrudeRelative == false )
        {
            mustStoreCmd("G91\n");
        }
        mustStoreCmd("G1 E-10\n");
        mustStoreCmd("G1 Z10\n");         
        if( isCoorRelative == false )
        {
            mustStoreCmd("G90\n"); 
        }
        if( isExtrudeRelative == true )
        {
            mustStoreCmd("M83\n");             
        }
    }
    else
    {		    
        if( isCoorRelative == false || isExtrudeRelative ==false )
        {
            mustStoreCmd("G91\n");
        }
        mustStoreCmd("G1 Z-10\n");
        mustStoreCmd("G1 E10\n");        
        if( isCoorRelative == false )
        {
            mustStoreCmd("G90\n"); 
        }
        if( isExtrudeRelative == true )
        {
            mustStoreCmd("M83\n");             
        }
    }
    resumeToPause(is_pause);
	return true;
}




void reValueNozzle(void)
{
    GUI_DispDec(126+12*2, 130, heatGetCurrentTemp(NOZZLE0), 3, 1, RIGHT);
    GUI_DispDec(126+12*6, 130, heatGetTargetTemp(NOZZLE0),  3, 1, LEFT);
}
void reValueBed(void)
{
    GUI_DispDec(246 + 2 * BYTE_WIDTH, 130, heatGetCurrentTemp(BED), 3, 1, RIGHT);
    GUI_DispDec(246 + 6 * BYTE_WIDTH, 130, heatGetTargetTemp(BED),  3, 1, LEFT);
}

void reDrawTime(void)
{
    u8  hour=infoPrinting.time/3600,
        min=infoPrinting.time%3600/60,
        sec=infoPrinting.time%60;
    
	GUI_DispChar(150,160,hour/10%10+'0',0);
    GUI_DispChar(150 + BYTE_WIDTH,160, hour%10+'0', 1);
	GUI_DispChar(150 + 3 * BYTE_WIDTH,160,min/10+'0',1);
    GUI_DispChar(150 + 4 * BYTE_WIDTH,160,min%10+'0',1);
    GUI_DispChar(150 + 6 * BYTE_WIDTH,160,sec/10+'0',1);
    GUI_DispChar(150 + 7 * BYTE_WIDTH,160,sec%10+'0',1);
}

void reDrawProgress(u8 progress)
{	
    GUI_FillRectColor(127, 81, 127+progress*2.27f, 119,BLUE);
    GUI_FillRectColor(127+progress*2.27f, 81, 354, 119,GRAY);
    GUI_DispDec(216,88,progress,3, 0, 1);
    GUI_DispChar(216+36,88,'%',0);		
}

extern SCROLL   titleScroll;
extern GUI_RECT titleRect;

//返回值供显示使用
//例如路径为 "SD:/test/测试/123.gcode"
//显示为 "123.gcode"
u8 *getCurGcodeName(char *path)
{
  int i=strlen(path);
  for(;path[i]!='/'&& i>0;i--){}
  return (u8* )(&path[i+1]);
}

void printingDrawPage(void)
{
    menuDrawPage(&printingItems);
//	Scroll_CreatePara(&titleScroll, infoFile.title,&titleRect);  //滚动显示路径名
    GUI_DispLenString(titleRect.x0, titleRect.y0, getCurGcodeName(infoFile.title),1, (titleRect.x1 - titleRect.x0)/BYTE_WIDTH );
	GUI_DispString(126,160,(u8* )"T:",0);
	GUI_DispChar(150+24,160,':',0);
	GUI_DispChar(150+60,160,':',0);
	GUI_DispString(126,130,(u8* )"E:",0);
	GUI_DispChar(126+12*5,130,'/',0);
	GUI_DispString(246,130,(u8* )"B:",0);
	GUI_DispChar(246+12*5,130,'/',0);
    reDrawProgress(infoPrinting.progress);
	reValueNozzle();
	reValueBed();
	reDrawTime();
}


void menuPrinting(void)	
{
	KEY_VALUES  key_num = KEY_IDLE;		
	u32         time=0;
	HEATER      nowHeat; 
	
    
    printingItems.items[KEY_ICON_0] = itemIsPause[infoPrinting.pause];
	printingDrawPage();
	
	while(infoMenu.menu[infoMenu.cur] == menuPrinting)
	{		
//        Scroll_DispString(&titleScroll,1,LEFT); //滚动显示路径名, 当路径名比较长的时候，会占用大量的时间，不建议使用

        if( infoPrinting.size != 0)
        {
            if(infoPrinting.progress!=limitValue(0,(uint64_t)infoPrinting.cur*100/infoPrinting.size,100))
            {
              infoPrinting.progress=limitValue(0,(uint64_t)infoPrinting.cur*100/infoPrinting.size,100);
              reDrawProgress(infoPrinting.progress);
            }	
        }
        else
        {
            if(infoPrinting.progress != 100)
            {
              infoPrinting.progress = 100;
              reDrawProgress(infoPrinting.progress);
            }	
        }            

        if(nowHeat.current[NOZZLE0]!=heatGetCurrentTemp(NOZZLE0) || nowHeat.target[NOZZLE0]!=heatGetTargetTemp(NOZZLE0))
        {
            nowHeat.current[NOZZLE0] = heatGetCurrentTemp(NOZZLE0); 
            nowHeat.target[NOZZLE0]  = heatGetTargetTemp(NOZZLE0);
            reValueNozzle();	
        }
        if(nowHeat.current[BED]!=heatGetCurrentTemp(BED) || nowHeat.target[BED]!=heatGetTargetTemp(BED))
        {
            nowHeat.current[BED] = heatGetCurrentTemp(BED); 
            nowHeat.target[BED]  = heatGetTargetTemp(BED);
            reValueBed();	
        }
        if(time!=infoPrinting.time)
        {
            time=infoPrinting.time;
            reDrawTime();
        }
        
        key_num = menuKeyGetValue();
        switch(key_num)
        {
            case KEY_ICON_0:
                setPrintPause(!isPause());
                break;
            case KEY_ICON_3:
                if(isPrinting())				
                  infoMenu.menu[++infoMenu.cur]=menuStopPrinting;	
                else
                {
                  exitPrinting();
                  infoMenu.cur--;
                }					
                break;
            case KEY_ICON_4:
                infoMenu.menu[++infoMenu.cur] = menuHeat;
                break;
            case KEY_ICON_5:
                infoMenu.menu[++infoMenu.cur] = menuFan;
                break;
            case KEY_ICON_6:
                infoMenu.menu[++infoMenu.cur] = menuSpeed;
                break;
            case KEY_ICON_7:
                infoMenu.menu[++infoMenu.cur] = menuBabyStep;
                break;
            default :break;
        }                
		loopProcess();
	}	
}

void exitPrinting(void)
{
//    infoPrinting.time = 0;	
//    infoPrinting.size = 0;
//    infoPrinting.cur = 0;
//    infoPrinting.printing = STATUS_FREE;
//    infoPrinting.progress = 0;
//    infoPrinting.pause = 0; 	
    memset(&infoPrinting,0,sizeof(PRINTING));
    ExitDir();	
}

void endPrinting(void)
{
    f_close(&infoPrinting.file);	
    powerFailedDelete();  
	endGcodeExecute();		
}


void completePrinting(void)
{
    infoPrinting.printing = 0;
    endPrinting();
}

void haltPrinting(void)
{
    clearCmdQueue();	
    heatSetIsWaiting(NOZZLE0, false);
    heatSetIsWaiting(BED, false);

    mustStoreCmd("G0 Z%d F3000\n", limitValue(0, (int)coordinateGetAxis(Z) + 10, Z_MAX_POS));
    mustStoreCmd("G28 X0 Y0\n");

    endPrinting();
    exitPrinting();
}


#define POPUP_CONFIRM_RECT {90,  210, 210, 260}
#define POPUP_CANCEL_RECT  {270, 210, 390, 260}

static BUTTON bottomBtn[] = {
//确定按钮                            正常状态                按下状态
{POPUP_CONFIRM_RECT, NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
{POPUP_CANCEL_RECT,  NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
};

static const GUI_RECT stopRect[] ={POPUP_CONFIRM_RECT, POPUP_CANCEL_RECT};

void menuStopPrinting(void)
{
	u16 key_num = IDLE_TOUCH;	
    
    TSC_ReDrawIcon = windowReDrawButton;
    bottomBtn[0].context = textSelect(LABEL_CONFIRM);
    bottomBtn[1].context = textSelect(LABEL_CANNEL);
    windowSetButton(bottomBtn, 2);    
    
    GUI_DrawWindow(&window, textSelect(LABEL_WARNING), textSelect(LABEL_STOP_PRINT));
    GUI_DrawButton(&bottomBtn[0], 0);
    GUI_DrawButton(&bottomBtn[1], 0);
	while(infoMenu.menu[infoMenu.cur] == menuStopPrinting)
	{
		key_num = KEY_GetValue(2, stopRect);
		switch(key_num)
		{
            case KEY_POPUP_CONFIRM:					
                haltPrinting();
                infoMenu.cur-=2;
            break;
            
            case KEY_POPUP_CANCEL:	
                infoMenu.cur--;
            break;		
		}
		loopProcess();
	}
}


/* 打印时，从sd卡获取gcode命令 */
void getGcodeFromFile(void)
{	
    bool 			sd_comment_mode = false;
    bool 			sd_comment_space= true;
    char 			sd_char;
	u8  			sd_count=0;
	u32 			br=0;
		
	if(isPrinting()==false)                       return;		
	
    powerFailedCache(infoPrinting.file.fptr);
	
	if(heatHasWaiting() || infoCmd.count || infoPrinting.pause )         return;
 	
//	if(popCleanNozzleCmd()==true)                 return;
//	
//	if(popPowerFailedCmd()==true)                 return;
    
	
	for(;infoPrinting.cur<infoPrinting.size;)
	{
        if(f_read(&infoPrinting.file,&sd_char,1,&br)!=FR_OK) break;
        
        infoPrinting.cur++;
        
        //Gcode命令解析
		if ( sd_char == '\n' )         //命令以 '\n' 为结束符
		{
			sd_comment_mode = false;   //for new command
			sd_comment_space= true;
			if(sd_count!=0)
			{
				infoCmd.queue[infoCmd.index_w][sd_count++] = '\n'; 
				infoCmd.queue[infoCmd.index_w][sd_count] = 0; //terminate string
				sd_count = 0; //clear buffer
				infoCmd.index_w = (infoCmd.index_w + 1) % CMD_MAX_LIST;
				infoCmd.count++;	
                break;			
			}
		}
		else if (sd_count >= CMD_MAX_CHAR - 2) {	}   //一帧命令过长，忽略后面的内容
		else 
		{
			if (sd_char == ';')             // 忽略 ;后面的注释
                sd_comment_mode = true;
			else 
			{
				if(sd_comment_space && (sd_char== 'G'||sd_char == 'M'))	       // 忽略空格，并且命令必须以 'G'或者 'M'开头，否则忽略此条命令
                    sd_comment_space=false;
				if (!sd_comment_mode && !sd_comment_space && sd_char != '\r')  // 将命令存入缓存队列中，等待发送
                    infoCmd.queue[infoCmd.index_w][sd_count++] = sd_char;
			}
		}
	}
  
	if((infoPrinting.cur>=infoPrinting.size) && isPrinting())  //打印完成
	{
        completePrinting();
	}
}



