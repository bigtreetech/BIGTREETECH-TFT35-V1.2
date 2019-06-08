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


static PRINTING infoPrinting;

static u32     update_time = M27_REFRESH * 100;
#ifdef ONBOARD_SD_SUPPORT
static bool    update_waiting = false;
#else
static bool    update_waiting = M27_WATCH_OTHER_SOURCES;
#endif


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

void printSetUpdateWaiting(bool isWaiting)
{
  update_waiting = isWaiting;
}


void startGcodeExecute(void)
{    

}

void endGcodeExecute(void)
{
  mustStoreCmd("G90\n");
  mustStoreCmd("G92 E0\n");
  for(TOOL i = BED; i < HEATER_NUM; i++)
  {
    mustStoreCmd("%s S0\n", heatCmd[i]);  
  }
  for(u8 i = 0; i < FAN_NUM; i++)
  {
    mustStoreCmd("%s S0\n", fanCmd[i]);  
  }
  mustStoreCmd("T0\n");
  mustStoreCmd("M18\n");
}


void menuBeforePrinting(void)
{
  long size = 0;
  switch (infoFile.source)
  {
  case BOARD_SD: // GCode from file on ONBOARD SD
    
     size = request_M23(infoFile.title+5);

  //  if( powerFailedCreate(infoFile.title)==false)
  //  {
  //
  //  }	  // FIXME: Powerfail resume is not yet supported for ONBOARD_SD. Need more work.

    if(size == 0)
    {
      infoMenu.cur--;		
      return;
    }

    infoPrinting.size  = size;
    infoPrinting.printing = true;

//    if(powerFailedExist())
//    {
      request_M24(0);
//    }
//    else
//    {
//      request_M24(infoBreakPoint.offset);
//    }
      printSetUpdateWaiting(true);

  #ifdef M27_AUTOREPORT
    request_M27(M27_REFRESH); 
  #else
    request_M27(0); 
  #endif

    infoHost.printing=true; // Global lock info on printer is busy in printing.

    break;
  case TFT_SD: // GCode from file on TFT SD
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
    break;
  }
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


  switch (infoFile.source)
  {
  case BOARD_SD:
    if(is_pause){
      request_M25();
    } else {
      request_M24(0);
    }
    break;
  case TFT_SD:
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
    break;
  }
  resumeToPause(is_pause);
  return true;
}

void reValueNozzle(void)
{
  GUI_DispString(120, 130, (u8* )heatDisplayID[heatGetCurrentToolNozzle()], 1);
  GUI_DispDec(120+12*3, 130, heatGetCurrentTemp(heatGetCurrentToolNozzle()), 3, 1, RIGHT);
  GUI_DispDec(120+12*7, 130, heatGetTargetTemp(heatGetCurrentToolNozzle()),  3, 1, LEFT);
}

void reValueBed(void)
{
  GUI_DispDec(250 + 2 * BYTE_WIDTH, 130, heatGetCurrentTemp(BED), 3, 1, RIGHT);
  GUI_DispDec(250 + 6 * BYTE_WIDTH, 130, heatGetTargetTemp(BED),  3, 1, LEFT);
}

void reDrawTime(void)
{
  u8  hour=infoPrinting.time/3600,
  min=infoPrinting.time%3600/60,
  sec=infoPrinting.time%60;

  GUI_DispChar(144,160,hour/10%10+'0',0);
  GUI_DispChar(144 + BYTE_WIDTH,160, hour%10+'0', 1);
  GUI_DispChar(144 + 3 * BYTE_WIDTH,160,min/10+'0',1);
  GUI_DispChar(144 + 4 * BYTE_WIDTH,160,min%10+'0',1);
  GUI_DispChar(144 + 6 * BYTE_WIDTH,160,sec/10+'0',1);
  GUI_DispChar(144 + 7 * BYTE_WIDTH,160,sec%10+'0',1);
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
  GUI_DispString(120,160,(u8* )"T:",0);
  GUI_DispChar(120+12*4,160,':',0);
  GUI_DispChar(120+12*6,160,':',0);
  
  GUI_DispString(120+12*2,130,(u8* )":",0);
  GUI_DispChar(120+12*6,130,'/',0);
  
  GUI_DispString(250,130,(u8* )"B:",0);
  GUI_DispChar(250+12*5,130,'/',0);
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

    if (nowHeat.T[heatGetCurrentToolNozzle()].current != heatGetCurrentTemp(heatGetCurrentToolNozzle()) 
     || nowHeat.T[heatGetCurrentToolNozzle()].target != heatGetTargetTemp(heatGetCurrentToolNozzle()))
    {
      nowHeat.T[heatGetCurrentToolNozzle()].current = heatGetCurrentTemp(heatGetCurrentToolNozzle());
      nowHeat.T[heatGetCurrentToolNozzle()].target = heatGetTargetTemp(heatGetCurrentToolNozzle());
      reValueNozzle();	
    }
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) 
     || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
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
  switch (infoFile.source)
  {
  case BOARD_SD:
    infoPrinting.printing = false;
    printSetUpdateWaiting(M27_WATCH_OTHER_SOURCES);
    break;
  case TFT_SD:
    f_close(&infoPrinting.file);	
    break;
  }
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
  switch (infoFile.source)
  {
  case BOARD_SD:
    request_M524();
    break;
  case TFT_SD:
    clearCmdQueue();	
    break;
  }

  heatClearIsWaiting();
  
  mustStoreCmd("G0 Z%d F3000\n", limitValue(0, (int)coordinateGetAxis(Z_AXIS) + 10, Z_MAX_POS));
  mustStoreCmd("G28 X0 Y0\n");

  endPrinting();
  exitPrinting();
}

static const GUI_RECT stopRect[] ={POPUP_RECT_DOUBLE_CONFIRM, POPUP_RECT_DOUBLE_CANCEL};

void menuStopPrinting(void)
{
  u16 key_num = IDLE_TOUCH;	

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_STOP_PRINT), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
 
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


// get gcode command from sd card
void getGcodeFromFile(void)
{	
  bool    sd_comment_mode = false;
  bool    sd_comment_space= true;
  char    sd_char;
  u8      sd_count=0;
  UINT    br=0;

  if(isPrinting()==false || infoFile.source == BOARD_SD)  return;
  
  powerFailedCache(infoPrinting.file.fptr);
  
  if(heatHasWaiting() || infoCmd.count || infoPrinting.pause )  return;
  
  if(moveCacheToCmd() == true) return;

  for(;infoPrinting.cur < infoPrinting.size;)
  {
    if(f_read(&infoPrinting.file, &sd_char, 1, &br)!=FR_OK) break;

    infoPrinting.cur++;

    //Gcode
    if (sd_char == '\n' )         //'\n' is end flag for per command
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
    else if (sd_count >= CMD_MAX_CHAR - 2) {	}   //when the command length beyond the maximum, ignore the following bytes
    else 
    {
      if (sd_char == ';')             //';' is comment out flag
        sd_comment_mode = true;
      else 
      {
        if(sd_comment_space && (sd_char== 'G'||sd_char == 'M'||sd_char == 'T'))  //ignore ' ' space bytes
          sd_comment_space = false;
        if (!sd_comment_mode && !sd_comment_space && sd_char != '\r')  //normal gcode
          infoCmd.queue[infoCmd.index_w][sd_count++] = sd_char;
      }
    }
  }

  if((infoPrinting.cur>=infoPrinting.size) && isPrinting())  // end of .gcode file
  {
    completePrinting();
  }
}

void loopCheckPrinting(void)
{
#if defined ONBOARD_SD_SUPPORT && !defined M27_AUTOREPORT   
  static u32  nowTime=0;

  do
  {  /* WAIT FOR M27	*/
    if(update_waiting == true)                {nowTime=OS_GetTime();break;}
    if(OS_GetTime()<nowTime+update_time)       break;

    if(storeCmd("M27\n")==false)               break;

    nowTime=OS_GetTime();
    update_waiting=true;
  }while(0);
#endif    
}




