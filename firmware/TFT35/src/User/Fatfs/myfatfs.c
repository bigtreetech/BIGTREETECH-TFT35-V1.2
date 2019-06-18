#include "myfatfs.h"
#include "includes.h"

FATFS getFileFs; /* FATFS work area*/

/* 
 mount SD Card from Fatfs
 true: mount ok
 false: mount failed
*/
bool mountSDCard(void)
{
  return (f_mount(&getFileFs,"SD:",1) == FR_OK);
}

static uint32_t date=0; 
static FILINFO  finfo;
static uint16_t len = 0;
/* 
 scanf gcode file in current path
 true: scanf ok
 false: opendir failed
*/
bool scanPrintFilesFatFs(void)
{
  DIR     dir;
  uint8_t i=0;

  clearInfoFile();

  if (f_opendir(&dir, infoFile.title) != FR_OK) return false;

  for(;;)
  {
    if(f_readdir(&dir,&finfo) !=FR_OK||finfo.fname[0]==0)       break;       /* 锟斤拷前目录锟窖撅拷锟斤拷锟斤拷锟斤拷锟� */
    if((finfo.fattrib&AM_HID) != 0)                             continue;    /* 锟斤拷锟斤拷锟侥硷拷锟斤拷锟斤拷锟斤拷 */
    if(infoFile.f_num>=FILE_NUM && infoFile.F_num>=FOLDER_NUM)  break;       /* 锟侥硷拷锟叫猴拷锟侥硷拷锟斤拷锟斤拷锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟� */
    
    len=strlen(finfo.fname)+1;
    if((finfo.fattrib&AM_DIR) == AM_DIR)      /* 锟侥硷拷锟斤拷 */
    {
      if(infoFile.F_num>=FOLDER_NUM)                            continue;   /* 锟侥硷拷锟斤拷锟窖筹拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 */

      infoFile.folder[infoFile.F_num]=malloc(len);
      if(infoFile.folder[infoFile.F_num]==NULL)
      break;
      memcpy(infoFile.folder[infoFile.F_num++],finfo.fname,len);
    }
    else                                             /* 锟侥硷拷 */
    {
      if(infoFile.f_num>=FILE_NUM)                              continue; /* Gcode锟窖筹拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 */\
      
      if(strstr(finfo.fname,".gcode")==NULL)                    continue; /* 锟斤拷锟斤拷Gcode锟侥硷拷 */

      infoFile.file[infoFile.f_num]=malloc(len);
      if(infoFile.file[infoFile.f_num]==NULL)                   break;
      memcpy(infoFile.file[infoFile.f_num++],finfo.fname,len);
    }		
  }

  /* 锟斤拷锟侥硷拷锟叫碉拷锟斤拷锟斤拷锟斤拷 */
  for(i=0;i<infoFile.F_num/2;i++)
  {
    char *temp=infoFile.folder[i];		
    infoFile.folder[i]=infoFile.folder[infoFile.F_num-i-1];
    infoFile.folder[infoFile.F_num-i-1]=temp;
  }
  /* 锟斤拷锟侥硷拷锟斤拷锟斤拷锟斤拷锟斤拷 */
  for(i=0;i<infoFile.f_num/2;i++)
  {
    char *temp=infoFile.file[i];		
    infoFile.file[i]=infoFile.file[infoFile.f_num-i-1];
    infoFile.file[infoFile.f_num-i-1]=temp;
  }
  return true;
}





/* 
锟斤拷锟斤拷:锟斤拷锟斤拷使锟斤拷,锟斤拷示锟侥硷拷锟斤拷锟揭伙拷胃锟斤拷牡锟绞憋拷锟�
* /
void GUI_DispDate(uint16_t date, uint16_t time)
{
  char buf[100];
  static uint8_t i=0;
  sprintf(buf,"%d/%d/%d--%d:%d:%d",1980+(date>>9),(date>>5)&0xF,date&0x1F,time>>11,(time>>5)&0x3F,time&0x1F);
  GUI_DispString(0,i,(uint8_t* )buf,0);
  i+=16;
}
*/


/* 
锟斤拷锟斤拷:锟斤拷取路锟斤拷path 锟斤拷锟斤拷锟铰碉拷Gcode锟侥硷拷
锟斤拷锟斤拷值: true锟斤拷锟揭碉拷锟斤拷锟斤拷Gcode锟侥硷拷  false 未锟揭碉拷
*/
bool Get_NewestGcode(const TCHAR* path)
{
  DIR     dirs;
  char	status = 0;	

  if (f_opendir(&dirs, path) != FR_OK) return false;

  len=strlen(path); 
  while (f_readdir(&dirs, &finfo) == FR_OK) 
  {
    if(finfo.fname[0]==0)                           break;							//锟窖撅拷锟斤拷锟斤拷锟斤拷 锟斤拷路锟斤拷锟斤拷锟斤拷锟叫碉拷锟侥硷拷锟皆硷拷锟侥硷拷锟斤拷 
    if((finfo.fattrib&AM_HID) != 0)                 continue;          //锟斤拷锟斤拷锟侥硷拷锟斤拷锟斤拷锟斤拷哦

    if((finfo.fattrib&AM_DIR) == AM_DIR)                            //锟侥硷拷锟斤拷
    {				
      char  *nextdirpath = malloc(len+strlen(finfo.fname)+2);
      if(nextdirpath==NULL)                         break;							//heap锟节存不锟姐，锟斤拷锟斤拷循锟斤拷

      strcpy(nextdirpath, path);
      strcat(nextdirpath,"/");
      strcat(nextdirpath, finfo.fname);	

      status|=Get_NewestGcode(nextdirpath);	
      free(nextdirpath);	
      nextdirpath=NULL;
    }
    else
    {
      if(strstr(finfo.fname,".gcode")==NULL)        continue;					//锟斤拷锟斤拷Gcode锟侥硷拷锟斤拷锟斤拷锟斤拷
      if(((finfo.fdate <<16)|finfo.ftime) < date)   continue;         //时锟斤拷锟斤拷锟揭伙拷锟斤拷傻牟锟斤拷锟斤拷锟�

      date=(finfo.fdate <<16)|finfo.ftime;
      resetInfoFile();

      if(len+strlen(finfo.fname)+2>MAX_PATH_LEN)    break;//锟节存不锟姐，锟斤拷锟斤拷循锟斤拷

      strcpy(infoFile.title,path);
      strcat(infoFile.title,"/");
      strcat(infoFile.title,finfo.fname);
      status = 1;
    }
  }
  return status;
}
