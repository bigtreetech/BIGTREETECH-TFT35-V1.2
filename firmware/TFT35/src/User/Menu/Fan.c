#include "fan.h"
#include "includes.h"


//1个title(标题), ITEM_PER_PAGE个item(图标+标签) 
const MENUITEMS fanItems = {
//   title
LABEL_FAN,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_FAN_FULL_SPEED,       LABEL_FAN_FULL_SPEED},
  {ICON_FAN_HALF_SPEED,       LABEL_FAN_HALF_SPEED},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_BACK,                 LABEL_BACK},}
};


static u8   fanSpeed = 0;
static bool send_waiting = false;

void fanSetSpeed(u8 speed)
{
  fanSpeed = speed;
}

u8 fanGetSpeed(void)
{
  return fanSpeed;
}
/* 设置是否已经发送风扇 */
void fanSetSendWaiting(bool isWaiting)
{
  send_waiting = isWaiting;
}

void showFanSpeed(void)
{
  GUI_DispString(210,85,(u8 *)"FAN",0);
  GUI_DispDec(210,110,fanSpeed,3,0,LEFT);
}

void fanSpeedReDraw(void)
{
  GUI_DispDec(210,110,fanSpeed,3,1,LEFT);
}

void menuFan(void)
{
  u8 nowFanSpeed = fanSpeed;
  KEY_VALUES key_num = KEY_IDLE;	

  menuDrawPage(&fanItems);
  showFanSpeed();
  while(infoMenu.menu[infoMenu.cur] == menuFan)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {            
      case KEY_ICON_0: if(fanSpeed > 0)   fanSpeed--; break;
      case KEY_ICON_3: if(fanSpeed < 255) fanSpeed++; break;
      case KEY_ICON_4: fanSpeed = 255; break;
      case KEY_ICON_5: fanSpeed = 127; break;
      case KEY_ICON_6: fanSpeed = 0;   break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default:break;            
    }
    if(nowFanSpeed != fanSpeed)
    {
      nowFanSpeed = fanSpeed;
      fanSpeedReDraw();
      if(send_waiting != true)
      {
        send_waiting = true;
        storeCmd("M106 ");
      }
    }        
    loopProcess();
  }
}
