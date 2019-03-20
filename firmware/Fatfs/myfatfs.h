#ifndef _MYFATFS_H_
#define _MYFATFS_H_


#include "stm32f10x.h"
#include "stdbool.h"

#define FOLDER_NUM  255
#define FILE_NUM    255

#define MAX_PATH_LEN 1024

typedef struct 
{
	char  title[MAX_PATH_LEN];			    //路径最长不超过1024字节
	char  *folder[FOLDER_NUM];				//最多255个文件夹
	char  *file[FILE_NUM];					//最多255个文件
	uint16_t F_num;
	uint16_t f_num;
	uint16_t cur_page;					        //当前显示的第几页
}MYFILE;	
extern MYFILE  infoFile;


bool    mountSDCard(void);
bool    scanPrintFiles(void);
bool    EnterDir(char *nextdir);
void    ExitDir(void);
bool    IsRootDir(void);
void    clearInfoFile(void);
void    resetInfoFile(void);
bool    Get_NewestGcode(const char* path);

#endif 




















