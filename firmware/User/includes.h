#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "stm32f10x.h"
#include "timer.h"
#include "delay.h"

#include "lcd.h"
#include "GUI.h"
#include "language.h"

#include "usart.h"
#include "spi.h"
#include "sw_spi.h"

#include "sd.h"
#include "w25qxx.h"
#include "xpt2046.h"

#include "photo.h"
#include "touch_process.h"
#include "interfaceCmd.h"
#include "coordinate.h"
#include "ff.h"
#include "myfatfs.h"
#include "flashStore.h"


//menu
#include "menu.h"
#include "Mainpage.h"

#include "Heat.h"
#include "Move.h"
#include "Home.h"
#include "Print.h"
#include "Printing.h"
#include "Speed.h"
#include "BabyStep.h"

#include "Extrude.h"
#include "Fan.h"
#include "Settings.h"
#include "PowerFailed.h"

#include "Popup.h"


#define LOGO_ADDR            0x0
#define ICON_ADDR(num)      ((num)*0x5000+0x4B000)

#define BYTE_ADDR           0x200000
#define WORD_ADDR           0x300000

#define UNI2OEM_ADDR	    0x600000
#define OEM2UNI_ADDR	    0x700000



#define MAX_MENU_DEPTH 10
typedef void (*FP_MENU)(void); 

typedef struct
{
	FP_MENU menu[MAX_MENU_DEPTH];
	u8      cur;
}MENU;

extern MENU infoMenu;    //菜单结构体
enum
{
    ICON_BACKGROUND = -1,
    ICON_HEAT = 0,
    ICON_MOVE,
    ICON_HOME,
    ICON_PRINT,
    ICON_EXTRUDE,
    ICON_FAN,
    ICON_SETTINGS,
    ICON_LEVELING,
    
    ICON_INC,
    ICON_DEC,
    ICON_NOZZLE,
    ICON_BED,
    ICON_1_DEGREE,
    ICON_5_DEGREE,
    ICON_10_DEGREE,
    ICON_STOP,
    ICON_BACK,
    ICON_X_INC,
    ICON_Y_INC,
    ICON_Z_INC,
    ICON_01_MM,
    ICON_1_MM,
    ICON_10_MM,
    ICON_X_DEC,
    ICON_Y_DEC,
    ICON_Z_DEC,
    
    ICON_X_HOME,
    ICON_Y_HOME,
    ICON_Z_HOME,
    
    ICON_FOLDER,
    ICON_FILE,
    ICON_PAGE_UP,
    ICON_PAGE_DOWN,
    ICON_PAUSE,
    ICON_RESUME,
    
    ICON_LOAD,
    ICON_UNLOAD,
    ICON_SLOW_SPEED,
    ICON_NORMAL_SPEED,
    ICON_FAST_SPEED,
    ICON_E_1_MM,
    ICON_E_5_MM,
    ICON_E_10_MM,
    
    ICON_FAN_FULL_SPEED,
    ICON_FAN_HALF_SPEED,
    ICON_POWER_OFF,
    ICON_LANGUAGE,
    ICON_TOUCHSCREEN_ADJUST,
    ICON_SCREEN_INFO,
    ICON_DISCONNECT,
    ICON_BAUDRATE,
    
    ICON_RESERVE_1,
    ICON_RESERVE_2,
    ICON_PERCENTAGE,
    ICON_BABYSTEP,
    ICON_001_MM,
    ICON_RESERVE_5,
    ICON_RESERVE_6,
    ICON_RESERVE_7,
    ICON_RESERVE_8,
    ICON_RESERVE_9,
    ICON_RESERVE_10,
    ICON_RESERVE_11,
    ICON_RESERVE_12,
    ICON_RESERVE_13,
    ICON_RESERVE_14,
};



typedef struct
{	
	bool wait;          //发送的命令是否有应答		0：有应答  1：未应答、等待应答后才能继续发送下一条命令
	bool rx_ok;
	bool connected;     //1:已连接打印机   0：未连接打印机
}HOST;

extern HOST infoHost;

#endif

