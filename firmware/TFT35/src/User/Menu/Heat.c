#include "Heat.h"
#include "includes.h"

//1��title(����), ITEM_PER_PAGE��item(ͼ��+��ǩ) 
MENUITEMS heatItems = {
//  title
LABEL_HEAT,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_BACK,                 LABEL_BACK},}
};


const ITEM itemTool[] = {
// icon                       label
  {ICON_BED,                  LABEL_BED},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
};    

#define ITEM_DEGREE_NUM 3
const ITEM itemDegree[ITEM_DEGREE_NUM] = {
// icon                       label
  {ICON_1_DEGREE,             LABEL_1_DEGREE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_10_DEGREE,            LABEL_10_DEGREE},
};
const  u8 item_degree[ITEM_DEGREE_NUM] = {1, 5, 10};
static u8 item_degree_i = 1;

const u16   heat_max_temp[] = HEAT_MAX_TEMP;
const char* toolID[] = HEAT_SIGN_ID;
const char* const heatDisplayID[] = HEAT_DISPLAY_ID;
const char* heatCmd[] = HEAT_CMD;
const char* heatWaitCmd[] = HEAT_WAIT_CMD;

static HEATER  heater = {{0}, NOZZLE0, NOZZLE0};
static u32     update_time = 300;
static bool    update_waiting = false;
static bool    send_waiting[HEATER_NUM];

/*����Ŀ���¶�*/
void heatSetTargetTemp(TOOL tool,u16 temp)
{
  heater.T[tool].target = temp;
}

/*��ȡĿ����¶�*/
u16 heatGetTargetTemp(TOOL tool)
{
  return heater.T[tool].target;
}

/* ���õ�ǰ���¶� */
void heatSetCurrentTemp(TOOL tool, s16 temp)
{
  heater.T[tool].current = limitValue(-99, temp, 999);
}

/* ��ȡ��ǰ���¶� */
s16 heatGetCurrentTemp(TOOL tool)
{
  return heater.T[tool].current;
}

/* �Ƿ�ȴ����������� */
bool heatGetIsWaiting(TOOL tool)
{
  return heater.T[tool].waiting;
}

/* ��ѯ�Ƿ�����Ҫ�ȴ��ļ����� */
bool heatHasWaiting(void)
{
  TOOL i;
  for(i = BED; i < HEATER_NUM; i++)
  {
    if(heater.T[i].waiting == true)
    return true;
  }
  return false;
}

/* �����Ƿ�ȴ����������� */
void heatSetIsWaiting(TOOL tool, bool isWaiting)
{
  heater.T[tool].waiting = isWaiting;
  if(isWaiting == true)
  {
    update_time = 100;
  }
  else if(heatHasWaiting() == false)
  {
    update_time = 300;		
  }
}

void heatClearIsWaiting(void)
{
  for(TOOL i = BED; i < HEATER_NUM; i++)
  {
    heater.T[i].waiting = false;
  }
  update_time = 300;
}

/* ���õ�ǰ����ͷ�����ȴ� */
void heatSetCurrentTool(TOOL tool)
{
  if(tool >= HEATER_NUM) return;
  heater.tool = tool;
}
/* ��ȡ��ǰ����ͷ�����ȴ� */
TOOL heatGetCurrentTool(void)
{
  return heater.tool;
}

/* ���õ�ǰ���ĸ���ͷ*/
void heatSetCurrentToolNozzle(TOOL tool)
{
  if(tool >= HEATER_NUM && tool < NOZZLE0) return;
  heater.nozzle = tool;
  heater.tool = tool;
}
/* ��ȡ��ǰ���Ǹ���ͷ*/
TOOL heatGetCurrentToolNozzle(void)
{
  return heater.nozzle;
}

/* ���ò�ѯ�¶ȵ�ʱ���� */
void heatSetUpdateTime(u32 time)
{
  update_time=time;
}
/* ���õ�ǰ�Ƿ���Ҫ��ѯ�¶� */
void heatSetUpdateWaiting(bool isWaiting)
{
  update_waiting = isWaiting;
}

/* �����Ƿ��Ѿ����ͼ������� */
void heatSetSendWaiting(TOOL tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void showTemperature(void)
{
  const GUI_RECT rect = {220, 90, 220+3*BYTE_WIDTH, 90+BYTE_HEIGHT};
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)heatDisplayID[heater.tool],1);
  GUI_DispDec(232-12*3,120,heater.T[heater.tool].current,3,1,RIGHT);
  GUI_DispChar(232,120,'/',0);
  GUI_DispDec(232+12,120,heater.T[heater.tool].target,3,1,LEFT);
}

void currentReDraw(void)
{
  GUI_DispDec(232-12*3,120,heater.T[heater.tool].current,3,1,RIGHT);
}

void targetReDraw(void)
{
  GUI_DispDec(232+12,120,heater.T[heater.tool].target,3,1,LEFT);
}

void menuHeat(void)
{
  HEATER      nowHeater = heater;
  KEY_VALUES  key_num = KEY_IDLE;

  update_time=100;

  menuDrawPage(&heatItems);
  showTemperature();

  while(infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if(heater.T[heater.tool].target > 0)
        {
          heater.T[heater.tool].target = 
            limitValue( 0, 
                        heater.T[heater.tool].target - item_degree[item_degree_i], 
                        heat_max_temp[heater.tool]);
        }
        break;
      
      case KEY_ICON_3:
        if(heater.T[heater.tool].target < heat_max_temp[heater.tool])
        {
          heater.T[heater.tool].target = 
            limitValue( 0, 
                        heater.T[heater.tool].target + item_degree[item_degree_i], 
                        heat_max_temp[heater.tool]);
        }
        break;
        
      case KEY_ICON_4:
        nowHeater.tool = heater.tool = (TOOL)((heater.tool+1) % HEATER_NUM);
        heatItems.items[key_num] = itemTool[heater.tool];
        menuDrawItem(&heatItems.items[key_num], key_num);
        showTemperature();
        break;
      
      case KEY_ICON_5:
        item_degree_i = (item_degree_i+1) % ITEM_DEGREE_NUM;
        heatItems.items[key_num] = itemDegree[item_degree_i];
        menuDrawItem(&heatItems.items[key_num], key_num);
        break;
      
      case KEY_ICON_6:
        heater.T[heater.tool].target = 0;
        break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default :
        break;
    }

    if(nowHeater.tool != heater.tool)
    {
      nowHeater.tool = heater.tool;
      showTemperature();
    }
    if(nowHeater.T[heater.tool].current != heater.T[heater.tool].current)
    {      
      nowHeater.T[heater.tool].current = heater.T[heater.tool].current;
      currentReDraw();
    }
    if(nowHeater.T[heater.tool].target != heater.T[heater.tool].target)
    {
      targetReDraw();
    }
    
    for(TOOL i = BED; i < HEATER_NUM; i++)
    {
      if(nowHeater.T[i].target != heater.T[i].target)
      {
        nowHeater.T[i].target = heater.T[i].target;
        if(send_waiting[i] != true)
        {
          send_waiting[i] = true;
          storeCmd("%s ",heatCmd[i]);
        }
      }
    }
    

    loopProcess();
  }
  if(heatHasWaiting()==false)
  update_time=300;
}


void loopCheckHeater(void)
{
  u8 i;
  static u32  nowTime=0;

  do
  {  /* ��ʱ����M105��ѯ�¶�	*/
    if(update_waiting == true)                {nowTime=OS_GetTime();break;}
    if(OS_GetTime()<nowTime+update_time)       break;

    if(storeCmd("M105\n")==false)              break;

    nowTime=OS_GetTime();
    update_waiting=true;
  }while(0);

  /* ��ѯ��Ҫ�ȴ��¶������ļ��������Ƿ�ﵽ�趨�¶� */
  for(i=0; i<HEATER_NUM; i++)
  {
    if (heater.T[i].waiting == false)                                   continue;
    if (i==BED)
    {
      if (heater.T[BED].current+2 <= heater.T[BED].target)              continue;
    }
    else
    {
      if (inRange(heater.T[i].current, heater.T[i].target, 2) != true)  continue;
    }

    heater.T[i].waiting = false;
    if(heatHasWaiting() == true)                                        continue;

    if(infoMenu.menu[infoMenu.cur] == menuHeat)                         break;
    update_time=300;
  }
}





