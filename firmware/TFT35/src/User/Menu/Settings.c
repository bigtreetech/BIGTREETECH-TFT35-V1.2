#include "Settings.h"
#include "includes.h"


SETTINGS infoSettings;

#if HARDWARE_VERSION == V1_0
  #define VER  "V1.0"
#elif HARDWARE_VERSION == V1_1
  #define VER  "V1.1"
#elif HARDWARE_VERSION == V1_2
  #define VER  "V1.2"
#endif


void menuInfo(void)
{
  GUI_Clear(BLACK);

  GUI_DispString(50,130,(u8 *)"Board   : BIGTREETECH_TFT35_"VER,0);
  GUI_DispString(50,160,(u8 *)"Firmware: "VER"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__,0);

  while(!isPress()) loopProcess();
  while(isPress())  loopProcess();

  infoMenu.cur--;
}

void menuDisconnect(void)
{
  GUI_Clear(BLACK);
  GUI_DispStringInRect(95,130,385,180,textSelect(LABEL_DISCONNECT_INFO),0);

  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x00000440;// PA9��PA10 ��������

  while(!isPress());
  while(isPress());
  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000008B0;// PA9 �������   PA10��������
  infoMenu.cur--;
}



//1��title(����), ITEM_PER_PAGE��item(ͼ��+��ǩ) 
MENUITEMS settingsItems = {
//   title
LABEL_SETTINGS,
// icon                       label
 {{ICON_POWER_OFF,            LABEL_POWER_OFF},
  {ICON_LANGUAGE,             LABEL_LANGUAGE},
  {ICON_TOUCHSCREEN_ADJUST,   LABEL_TOUCHSCREEN_ADJUST},
  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};


#define ITEM_BAUDRATE_NUM 2
const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
//   icon                       label
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_250000},
};
const  u32 item_baudrate[ITEM_BAUDRATE_NUM] = {115200,250000};
static u8  item_baudrate_i = 0;


void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for(u8 i=0; i<ITEM_BAUDRATE_NUM; i++)
  {
    if(infoSettings.baudrate == item_baudrate[i])
    {
      item_baudrate_i = i;
      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
    }
  }	
  menuDrawPage(&settingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:    mustStoreCmd("M81\n");                                break;
      
      case KEY_ICON_1: 
        infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_2:    
        TSC_Calibration();
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;
      
      case KEY_ICON_5:
        item_baudrate_i = (item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;                
        settingsItems.items[key_num] = itemBaudrate[item_baudrate_i];
        menuDrawItem(&settingsItems.items[key_num], key_num);
        infoSettings.baudrate = item_baudrate[item_baudrate_i];
        USART1_Config(infoSettings.baudrate);
        break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    loopProcess();		
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
