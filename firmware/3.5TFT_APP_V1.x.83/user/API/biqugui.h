#ifndef _BIQU_GUI_H_
#define _BIQU_GUI_H_
#include "includes.h"
#include "ff.h"
#include "photo.h"
#include "language.h"



#define LANGUAGE_ADDR   (0x0803F000-0x800)	
#define ACOUNT(a)	sizeof(a)/sizeof(a[0])



enum
{
	MENU_SPEED=53,
	MENU_BABY=54,
	MENU_MAIN=56,
	MENU_PRINTING,
	MENU_HEAT=0,
	MENU_MOVE,
	MENU_HOME,
	MENU_PRINT,
	MENU_EXTRUDER,
	MENU_FAN,
	MENU_SETTING,
	MENU_OUTAGE,
	
};

#define BED							BED_MAXTEMP
#define NOZZLE					NOZZLE_MAXTEMP





#define CMD_POSITION		 				"M114\n"
#define CMD_TEMP 							 	"M105\n"
#define CMD_AUTO_HOME 				  "G28\n"
#define CMD_AUTO_LEVELING				"G29\n"
#define CMD_CLOSE_ST						"M84\n"
#define CMD_E_ABSOLUTE					"M82\n"
#define CMD_E_RELATIVE					"M83\n"
#define CMD_XYZ_ABSOLUTE				"G90\n"
#define CMD_XYZ_RELATIVE				"G91\n"
#define CMD_WAIT_TEMP						"M116\n"

#define CMD_MOVE_SPEED					"M220"
#define CMD_EXTRUDE_SPEED				"M221"



typedef struct 
{
	u16 	get_time;
	bool	wait_M105;	
	bool 	update;
}INFOHEATUPDATE;

typedef struct TAGHEAT
{
	u16 cur_nozzle;
	u16 cur_bed;
	u16 tag_nozzle;
	u16 tag_bed;
	u16 type;
	u8 heat_unit;
}INFOHEAT;

typedef struct TAGMOVE
{
	u8 move_unit; 
}INFOMOVE;

typedef struct TAGEXTRUDE
{
	s16 len_totel;
	u16 speed_unit;
	u8 move_unit;
}INFOEXTRUDE;

typedef struct TAGFAN
{
	u8 fan_speed;
}INFOFAN;

typedef struct TAGSETTING
{
	u8 language;
	u32 baudrate;
}INFOSETTING;

typedef struct TAGPRINT
{
	char file[_MAX_LFN];		//要打印的文件
	char dispfile[40];			//标题栏显示的文件名
	u8 back_icon[5];
}INFOPRINT;
typedef struct TAGPRINTING
{
	u32 	time;								//打印时间  单位：秒
	float	baby_step;
	u16 	xyz_speed;								
	u16		e_speed;
	u8 		speed_type;
	u8 	 	speed_unit;
	u8		baby_unit;
	u8	 	wait;
	bool 	pause;								//1：暂停  0：未暂停
	bool	ok;									//打印标志  1：正在打印  0：未打印
}INFOPRINTING;

#define FOLDER_NUM 255
#define FILE_NUM	255
typedef struct TAGFILE
{
	char *title[10];
	u8 cur_nesting;
	char *folder[FOLDER_NUM];
	char *file[FILE_NUM];
	u8 F_num;
	u8 f_num;
	u8 cur_page;
}INFOMYFILE;

extern GUI_RECT menuRect[8];
extern GUI_RECT menuRectPrint[13];
extern GUI_RECT menuRectOutage[2];
extern u8 cur_view;


extern INFOHEATUPDATE infoHeatUpdate;

extern INFOHEAT   	infoHeat;										//cur_nozzle，cur_bed，tag_nozzle，tag_bed，type
extern INFOMOVE 		infoMove;												//move_unit
extern INFOEXTRUDE	infoExtrude;										//len_totel,speed_unit,move_unit
extern INFOFAN			infoFan;												//fan_speed
extern INFOSETTING	infoSetting;										//language
extern INFOPRINT		infoPrint;											//file,dispfile,back_icon
extern INFOPRINTING infoPrinting;										//time,xyz_speed,e_speed,pause,ok
extern INFOMYFILE		infoMyfile;											//title,cur_nesting,folder,file,Fnum,fnum,cur_page


extern bool press;
extern u8 	err_flag;
extern u8 	outage_save;


extern FILINFO info;
extern FIL fnew;
extern FRESULT res;
extern UINT br;
extern FATFS myfs;	


void Read_Display_black(u8 i);

void infoDisplay( u8 *warning);
bool storeCmd(char *cmd);

void menuPicClear(void);
void mainMenuPic(void);
void mainMenu(void);


void heatValueDisplay(void);
void heatMenuPic(void);
bool heatStoreCmd(u16 type,u16 sendvalue);
void heatProcessKey(u8 key_num,u16 *display_tag);
void heatMenu(void);


void moveMenuPic(void);
void moveStoreCmd(char xyz, char change,u8 move_unit);
void moveMenu(void);


void homeMenuPic(void);
void homeMenu(void);


u8 Getfile_fromSD(void);
u8 printMenu(void);
void printingMenu(void);


void printingTransmissionPic(void);
void transmissionStoreCmd(char *transcmd,u16 transspeed);
void transmissionProcessKey(u8 key_num);


void extrudeMenuPic(void);
bool extrudeStoreCmd(char change,u8 extrude_unit,u16 extrude_speed);
void extrudeMenu(void);

void fanMenuPic(void);
bool fanStoreCmd(u8 fan_value);
void fanProcessKey(u8 key_num);
void fanMenu(void);

void settingMenuPic(void);
void settingMenu(void);

void moreMenuPic(void);
void moreMenu(void);

#endif
