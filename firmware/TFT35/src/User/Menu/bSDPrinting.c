#include "bSDPrinting.h"
#include "includes.h"

//1锟斤拷title(锟斤拷锟斤拷), ITEM_PER_PAGE锟斤拷item(图锟斤拷+锟斤拷签) 
MENUITEMS bSDprintingItems = {
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


const ITEM bSDitemIsPause[2] = {
// icon                       label
  {ICON_PAUSE,                LABEL_PAUSE},
  {ICON_RESUME,               LABEL_RESUME},
};

void menuBeforeBSDPrinting(void)
{

  GUI_Clear(BK_COLOR);
  int dbpos = 1;
  char *buf=malloc(1000);
  sprintf(buf, "Before printing:%s",infoFile.title+5);
  GUI_DispStringInRect(0,(BYTE_HEIGHT*dbpos),LCD_WIDTH,LCD_HEIGHT-(BYTE_HEIGHT*dbpos),(u8 *)buf,0);
  free(buf);

  long size = request_M23(infoFile.title+5);

  dbpos++;
  buf=malloc(1000);
  sprintf(buf, "OK Print Size:%lu",size);
  GUI_DispStringInRect(0,(BYTE_HEIGHT*dbpos),LCD_WIDTH,LCD_HEIGHT-(BYTE_HEIGHT*dbpos),(u8 *)buf,0);
  free(buf);
  Delay_ms(5000); 


  if(size == 0)
  {
    infoMenu.cur--;		
    return;
  }
 
  infoPrinting.size  = size;
  infoPrinting.printing = true;

  dbpos++;
  buf=malloc(1000);
  sprintf(buf, "Before start");
  GUI_DispStringInRect(0,(BYTE_HEIGHT*dbpos),LCD_WIDTH,LCD_HEIGHT-(BYTE_HEIGHT*dbpos),(u8 *)buf,0);
  free(buf);
  Delay_ms(5000); 

  request_M24(0);

  dbpos++;
  buf=malloc(1000);
  sprintf(buf, "Started");
  GUI_DispStringInRect(0,(BYTE_HEIGHT*dbpos),LCD_WIDTH,LCD_HEIGHT-(BYTE_HEIGHT*dbpos),(u8 *)buf,0);
  free(buf);
  Delay_ms(5000); 


#ifdef M27_AUTOREPORT
  request_M27(M27_REFRESH); 
#else
  request_M27(0); 
#endif

  dbpos++;
  buf=malloc(1000);
  sprintf(buf, "Ask for status every 2 seconds");
  GUI_DispStringInRect(0,(BYTE_HEIGHT*dbpos),LCD_WIDTH,LCD_HEIGHT-(BYTE_HEIGHT*dbpos),(u8 *)buf,0);
  free(buf);

  infoMenu.menu[infoMenu.cur] = menuBSDPrinting;
}


bool setBSDPrintPause(bool is_pause)
{
  if(!isPrinting())                       return false;
  if(infoPrinting.pause == is_pause)      return false;


  if(is_pause){
    request_M24(infoPrinting.cur);
  } else {
    request_M25();
  }

  infoPrinting.pause = is_pause;    
  return true;
}


void menuBSDPrinting(void)	
{
  KEY_VALUES  key_num = KEY_IDLE;		
  u32         time = 0;
  HEATER      nowHeat; 
  memset(&nowHeat, 0, sizeof(HEATER));
  
  bSDprintingItems.items[KEY_ICON_0] = bSDitemIsPause[infoPrinting.pause];
  printingDrawPage();

  while(infoMenu.menu[infoMenu.cur] == menuBSDPrinting)
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
        setBSDPrintPause(!isPause());
        break;
      
      case KEY_ICON_3:
        if(isPrinting())				
          infoMenu.menu[++infoMenu.cur]=menuStopBSDPrinting;	
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

void haltBSDPrinting(void)
{
  request_M524();
  endPrinting();
  exitPrinting();
}


static const GUI_RECT stopRect[] ={POPUP_RECT_DOUBLE_CONFIRM, POPUP_RECT_DOUBLE_CANCEL};


void menuStopBSDPrinting(void)
{
  u16 key_num = IDLE_TOUCH;	

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_STOP_PRINT), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANNEL));
 
  while(infoMenu.menu[infoMenu.cur] == menuStopPrinting)
  {
    key_num = KEY_GetValue(2, stopRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:					
      haltBSDPrinting();
      infoMenu.cur-=2;
      break;

      case KEY_POPUP_CANCEL:	
      infoMenu.cur--;
      break;		
    }
    loopProcess();
  }
}


