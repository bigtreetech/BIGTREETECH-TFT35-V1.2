#include "menu.h"
#include "includes.h"

bool inRange(int cur, int tag ,u8 range)
{
  if((cur<=tag+range)&&(cur>=tag-range))
    return true;
  return false;
}

int limitValue(int min, int value, int max)
{
  if(value<min)
    return min;
  if(value>max)
    return max;
  return value;
}

const GUI_RECT rect_of_key[ITEM_PER_PAGE*2]={
  //�˸�ͼ���������������
  {12,    60,     12+95,  155},
  {132,   60,     132+95, 155},
  {252,   60,     252+95,	155},
  {372,   60,     372+95,	155},
  {12,    190,    12+95 ,	285},
  {132,   190,    132+95,	285},
  {252,   190,    252+95,	285},
  {372,   190,    372+95,	285},

  //ÿ��ͼ������� �������� ��Ӧ������
  {0,     155,  120,    190},
  {120,   155,	240,	  190},
  {240,   155,	360,    190},
  {360,   155,	480,    190},
  {0,     285,	120,    320},
  {120,   285,	240,    320},
  {240,   285,	360,    320},
  {360,   285,	480,    320},
};

//���ͼ��֮��ķ�϶
void menuClearGaps(void)
{
  const GUI_RECT gaps[]={{0,0,480,60},{0,60,12,320},{107,60,132,320},{227,60,252,320},{347,60,372,320},{467,60,480,320}};

  for(int i=0; i < sizeof(gaps)/sizeof(gaps[0]); i++)
  GUI_ClearRect(gaps[i].x0, gaps[i].y0, gaps[i].x1, gaps[i].y1);
}

static const MENUITEMS * curMenuItems = NULL;   //��ǰ��ʾ�Ľ���(���⡢ͼ�ꡢ��ǩ)

void menuDrawItem(const ITEM * item, u8 positon)
{
  const GUI_RECT *rect = rect_of_key + positon;    
  if(item->icon != ICON_BACKGROUND)
    ICON_ReadDisplay(rect->x0, rect->y0, item->icon);
  else
    GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);

  rect = rect_of_key + ITEM_PER_PAGE + positon;
  GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);
  if(item->label != LABEL_BACKGROUND)
    GUI_DispStringInPrect(rect,textSelect(item->label),0);
}


static REMINDER reminder = {{0, 10, 480, 10 + BYTE_HEIGHT}, 0, STATUS_UNCONNECT, LABEL_UNCONNECTED};
static REMINDER busySign = {{LCD_WIDTH - 5, 0,LCD_WIDTH,5},0,STATUS_BUSY, LABEL_BUSY};


void reminderMessage(int16_t inf, SYS_STATUS status)   
{    
  reminder.inf = inf;
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf), 1);
  GUI_SetColor(WHITE);
  reminder.status = status;
  reminder.time = OS_GetTime();
}

void busyIndicator(SYS_STATUS status)
{
  if(status == STATUS_BUSY)
  {
    GUI_SetColor(YELLOW);
    GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
    GUI_SetColor(WHITE);
  }
  busySign.status = status;
  busySign.time = OS_GetTime();
}

void loopReminderClear(void)
{	
  switch(reminder.status)
  {
    case STATUS_IDLE:
      return;
    
    case STATUS_BUSY:
      if(infoCmd.count == CMD_MAX_LIST)
        return;
      break;
      
    case STATUS_UNCONNECT:  
      if(infoHost.connected == false)
        return;
      break;
      
    case STATUS_NORMAL:
      if(OS_GetTime()<reminder.time+200)
        return;
      break;
  }

  /* ���������Ϣ */		
  reminder.status = STATUS_IDLE;
  if(curMenuItems == NULL)
    return;
  menuDrawTitle(curMenuItems);
}

void loopBusySignClear(void)
{	
  switch(busySign.status)
  {
    case STATUS_IDLE:
      return;
    
    case STATUS_BUSY:
     if(OS_GetTime()<busySign.time+200)
        return;
    break;            
  }

  /* End Busy display sing */		
  busySign.status = STATUS_IDLE;
  GUI_SetColor(BK_COLOR);
  GUI_FillCircle(busySign.rect.x0, (busySign.rect.y1 - busySign.rect.y0) / 2, (busySign.rect.x1-busySign.rect.x0)/2);
  GUI_SetColor(WHITE);
}


void menuDrawTitle(const MENUITEMS * menuItems)
{
  GUI_ClearRect(10, 10, 470, 10 + BYTE_HEIGHT);
  
  if(menuItems->title != LABEL_BACKGROUND)
    GUI_DispString(10, 10, textSelect(menuItems->title),0);

  if(reminder.status == STATUS_IDLE) return;
  GUI_SetColor(RED);
  GUI_DispStringInPrect(&reminder.rect, textSelect(reminder.inf), 1);
  GUI_SetColor(WHITE);
}

//������������
void menuDrawPage(const MENUITEMS * menuItems)
{
  u8 i=0;
  curMenuItems = menuItems;
  TSC_ReDrawIcon = itemDrawIconPress;
  //    GUI_Clear(BLACK);
  menuClearGaps();     //ʹ�ô˺��������� GUI_Clear ��Ϊ����������ʱ����������
  menuDrawTitle(menuItems);
  for(i=0; i<ITEM_PER_PAGE; i++)
  {
    menuDrawItem(&menuItems->items[i], i);            
  }
}

//�а���ֵʱ��ͼ���ɫ�����ػ�
void itemDrawIconPress(u8 positon, u8 is_press)
{
  if(curMenuItems == NULL)                                  return;
  if(positon > KEY_ICON_7)                                  return;
  if(curMenuItems->items[positon].icon == ICON_BACKGROUND)  return;

  const GUI_RECT *rect = rect_of_key + positon;   
  if(is_press)   //����ʱ����
    ICON_PressedDisplay(rect->x0, rect->y0, curMenuItems->items[positon].icon);
  else           //�ɿ�ʱ�ػ�����ͼ��
    ICON_ReadDisplay(rect->x0, rect->y0, curMenuItems->items[positon].icon);
}


//��ȡ����ֵ
KEY_VALUES menuKeyGetValue(void)
{    
  return(KEY_VALUES)KEY_GetValue(sizeof(rect_of_key)/sizeof(rect_of_key[0]), rect_of_key);    
}

void loopProcess (void)
{
  getGcodeFromFile();                 //�Ӵ���ӡ���ļ��н�����Gcode����

  //    parseQueueCmd();                
  sendQueueCmd();                     //���������Ͷ����е�Gcode����
  
  parseACK();                         //�������յĴӻ�Ӧ����Ϣ	

  loopCheckHeater();			            //�¶���ص�����

  loopReminderClear();	              //��״̬������ʾ��Ϣ����ʱ���

  loopBusySignClear();                //Busy Indicator clear
}
