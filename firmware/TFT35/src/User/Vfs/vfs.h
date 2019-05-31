#ifndef _VFS_H_
#define _VFS_H_

#include "stdint.h"

#define FOLDER_NUM   255
#define FILE_NUM     255
#define MAX_PATH_LEN 1024

typedef enum {
    TFT_SD,
    BOARD_SD
}FS_SOURCE;

typedef struct 
{
	char title[MAX_PATH_LEN];			    //路锟斤拷锟筋长锟斤拷锟斤拷锟斤拷1024锟街斤拷
	char *folder[FOLDER_NUM];				//锟斤拷锟�255锟斤拷锟侥硷拷锟斤拷
	char *file[FILE_NUM];					//锟斤拷锟�255锟斤拷锟侥硷拷
	uint16_t F_num;
	uint16_t f_num;
	uint16_t cur_page;					    //锟斤拷前锟斤拷示锟侥第硷拷页
}MYFILE;	

extern MYFILE  infoFile;
extern FS_SOURCE sourceFile;

char mountFS(void);
char scanPrintFiles(void); 

void clearInfoFile(void);
void resetInfoFile(void);
char EnterDir(char *nextdir);
void ExitDir(void);
char IsRootDir(void);


#endif 
