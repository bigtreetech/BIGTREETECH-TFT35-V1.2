#include "vfs.h"
#include "includes.h"

MYFILE infoFile={"?:", {0}, {0}, 0, 0, 0, TFT_SD};

char mountFS(void)
{
  resetInfoFile();
  switch (infoFile.source)
  {
    case TFT_SD:
      return mountSDCard();
    case BOARD_SD:
      return mountGcodeSDCard();
  }
  return 0;
}

/* 

*/
void clearInfoFile(void)
{	
  uint8_t i=0;
  for (i=0; i<infoFile.F_num; i++)
  {
    free(infoFile.folder[i]);
    infoFile.folder[i] = 0;
  }
  for (i=0; i<infoFile.f_num; i++)
  {
    free(infoFile.file[i]);
    infoFile.file[i] = 0;
  }
  infoFile.F_num = 0;
  infoFile.f_num = 0;
}


/* 
infoFile
*/
void resetInfoFile(void)
{
  FS_SOURCE source = infoFile.source;
  clearInfoFile();
  memset(&infoFile, 0, sizeof(infoFile));
  infoFile.source = source;
  switch (source)
  {
    case TFT_SD:
      memcpy(infoFile.title, "SD:", 4);
      break;
    case BOARD_SD:
      memcpy(infoFile.title, "bSD:", 5);
      break;  
  }
}

// Modify
//static uint32_t date=0; 
//static FILINFO  finfo;
//static uint16_t len = 0;
/* 
锟斤拷锟斤拷:扫锟借当前路锟斤拷锟铰碉拷 锟缴达拷印锟侥硷拷
锟斤拷锟斤拷值: true扫锟斤拷晒锟� false扫锟斤拷失锟斤拷
*/
char scanPrintFiles(void)
{
  clearInfoFile();
  switch (infoFile.source)
  {
    case TFT_SD:
      return scanPrintFilesFatFs();
    case BOARD_SD:
      return scanPrintFilesGcodeFs();
  }
  return 0;
}


/* 
锟斤拷锟斤拷:锟斤拷锟斤拷锟斤拷一锟斤拷目录
锟斤拷锟斤拷值:true锟斤拷锟斤拷晒锟� false锟斤拷锟斤拷失锟斤拷
*/
char EnterDir(char *nextdir)
{
  if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return 0;
  strcat(infoFile.title,"/");
  strcat(infoFile.title,nextdir);
  return 1;
}


/* 
锟斤拷锟斤拷:锟斤拷锟斤拷锟斤拷一锟斤拷路锟斤拷
*/
void ExitDir(void)
{
  int i=strlen(infoFile.title);
  for(;i>0&&infoFile.title[i]!='/';i--)
  {	
  }
  infoFile.title[i]=0;
}


/* 
锟斤拷锟斤拷:锟斤拷前路锟斤拷锟角凤拷为 锟斤拷目录("0:")
锟斤拷锟斤拷值: true锟角革拷目录 false锟斤拷锟角革拷目录
*/
char IsRootDir(void)
{
  return !strchr(infoFile.title,'/');
}
