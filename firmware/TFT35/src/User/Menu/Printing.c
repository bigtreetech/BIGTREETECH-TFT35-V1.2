#include "Printing.h"
#include "includes.h"

//1锟斤拷title(锟斤拷锟斤拷), ITEM_PER_PAGE锟斤拷item(图锟斤拷+锟斤拷签) 
MENUITEMS printingItems = {
//  title
LABEL_BACKGROUND,
// icon                       label
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
// icon                       label
  {ICON_PAUSE,                LABEL_PAUSE},
  {ICON_RESUME,               LABEL_RESUME},
};


PRINTING infoPrinting;


//锟角凤拷锟斤拷锟节达拷印
bool isPrinting(void)
{
  return infoPrinting.printing;
}

//锟角凤拷锟斤拷锟斤拷停状态
bool isPause(void)
{
  return infoPrinting.pause;
}

//锟斤拷锟铰达拷印锟斤拷时
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
//锟斤拷锟矫达拷印锟侥硷拷锟斤拷锟杰达拷小
void setPrintSize(u32 size)
{
  infoPrinting.size = size;
}
//锟斤拷锟矫碉拷前锟窖达拷印锟侥达拷小
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

  }	
  powerFailedlSeek(&infoPrinting.file);

  infoPrinting.size  = f_size(&infoPrinting.file);
  infoPrinting.cur   = infoPrinting.file.fptr;
  infoPrinting.printing = true;

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

  bool isCoorRelative = coorGetRelative();
  bool isExtrudeRelative = eGetRelative();

  infoPrinting.pause = is_pause;    
  if(infoPrinting.pause)
  {             
    if( isCoorRelative == false)
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
    if( isCoorRelative == false)
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

//锟斤拷锟斤拷值锟斤拷锟斤拷示使锟斤拷
//锟斤拷锟斤拷路锟斤拷为 "SD:/test/锟斤拷锟斤拷/123.gcode"
//锟斤拷示为 "123.gcode"
u8 *getCurGcodeName(char *path)
{
  int i=strlen(path);
  for(; path[i]!='/'&& i>0; i--)
  {}
  return (u8* )(&path[i+1]);
}

void printingDrawPage(void)
{
  menuDrawPage(&printingItems);
  //	Scroll_CreatePara(&titleScroll, infoFile.title,&titleRect);  //锟斤拷锟斤拷锟斤拷示路锟斤拷锟斤拷
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
  u32         time = 0;
  HEATER      nowHeat; 
  memset(&nowHeat, 0, sizeof(HEATER));
  
  printingItems.items[KEY_ICON_0] = itemIsPause[infoPrinting.pause];
  printingDrawPage();

  while(infoMenu.menu[infoMenu.cur] == menuPrinting)
  {		
//    Scroll_DispString(&titleScroll,1,LEFT); //锟斤拷锟斤拷锟斤拷示路锟斤拷锟斤拷, 锟斤拷路锟斤拷锟斤拷锟饺较筹拷锟斤拷时锟津，伙拷占锟矫达拷锟斤拷锟斤拷时锟戒，锟斤拷锟斤拷锟斤拷使锟斤拷

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

  mustStoreCmd("G0 Z%d F3000\n", limitValue(0, (int)coordinateGetAxis(Z_AXIS) + 10, Z_MAX_POS));
  mustStoreCmd("G28 X0 Y0\n");

  endPrinting();
  exitPrinting();
}


#define POPUP_CONFIRM_RECT {90,  210, 210, 260}
#define POPUP_CANCEL_RECT  {270, 210, 390, 260}

static BUTTON bottomBtn[] = {
  //确锟斤拷锟斤拷钮                            锟斤拷锟斤拷状态                锟斤拷锟斤拷状态
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


/* 锟斤拷印时锟斤拷锟斤拷sd锟斤拷锟斤拷取gcode锟斤拷锟斤拷 */
void getGcodeFromFile(void)
{	
  bool    sd_comment_mode = false;
  bool    sd_comment_space= true;
  char    sd_char;
  u8      sd_count=0;
  UINT    br=0;

  if(isPrinting()==false)  return;
  
  powerFailedCache(infoPrinting.file.fptr);
  
  if(heatHasWaiting() || infoCmd.count || infoPrinting.pause )  return;
  
  if(moveCacheToCmd() == true) return;

  for(;infoPrinting.cur < infoPrinting.size;)
  {
    if(f_read(&infoPrinting.file, &sd_char, 1, &br)!=FR_OK) break;

    infoPrinting.cur++;

    //Gcode锟斤拷锟斤拷锟斤拷锟�
    if ( sd_char == '\n' )         //锟斤拷锟斤拷锟斤拷 '\n' 为锟斤拷锟斤拷锟斤拷
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
    else if (sd_count >= CMD_MAX_CHAR - 2) {	}   //一帧锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷院锟斤拷锟斤拷锟斤拷锟斤拷
    else 
    {
      if (sd_char == ';')             // 锟斤拷锟斤拷 ;锟斤拷锟斤拷锟阶拷锟�
        sd_comment_mode = true;
      else 
      {
        if(sd_comment_space && (sd_char== 'G'||sd_char == 'M'))	       // 锟斤拷锟皆空格，诧拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟� 'G'锟斤拷锟斤拷 'M'锟斤拷头锟斤拷锟斤拷锟斤拷锟斤拷源锟斤拷锟斤拷锟斤拷锟�
        sd_comment_space=false;
        if (!sd_comment_mode && !sd_comment_space && sd_char != '\r')  // 锟斤拷锟斤拷锟斤拷锟斤拷牖猴拷锟斤拷锟斤拷锟叫ｏ拷锟饺达拷锟斤拷锟斤拷
        infoCmd.queue[infoCmd.index_w][sd_count++] = sd_char;
      }
    }
  }

  if((infoPrinting.cur>=infoPrinting.size) && isPrinting())  //锟斤拷印锟斤拷锟�
  {
    completePrinting();
  }
}



