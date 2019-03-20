#include "includes.h"
#include "ff.h"
#include "myfatfs.h"

FATFS getFileFs; /* 文件系统分区, 确保此变量的内存一直存在 */

MYFILE  infoFile={"SD:",0,0,0,0,0,0};

/* 
功能:挂载SD卡
返回值: true成功 false失败
*/
bool mountSDCard(void)
{									
//	u8 timeout;

//	while(SD_Init())         /* 检测SD卡 */
//	{
//		Delay(50);
//		timeout++;
//		if(timeout>10)
//		{
//			return false;
//		}
//	}
	
	if(f_mount(&getFileFs,"SD:",1) != FR_OK)  /* 挂载文件系统 */
	{
		return false;
	}
	
	return true;
}


/* 
功能:清除infoFile 释放malloc申请的内存，避免内存泄漏
*/
void clearInfoFile(void)
{	
	u8 i=0;
	for(i=0;i<infoFile.F_num;i++)
	{
		free(infoFile.folder[i]);
		infoFile.folder[i]=0;
	}
	for(i=0;i<infoFile.f_num;i++)
	{
		free(infoFile.file[i]);
		infoFile.file[i]=0;
	}
	infoFile.F_num=0;
	infoFile.f_num=0;
}


/* 
功能:初始化infoFile
*/
void resetInfoFile(void)
{
    clearInfoFile();
    memset(&infoFile,0,sizeof(infoFile));
    memcpy(infoFile.title,"SD:",4);
}


static u32     date=0; 
static FILINFO finfo;
static u16     len;
/* 
功能:扫描当前路径下的 可打印文件
返回值: true扫描成功 false扫描失败
*/
bool scanPrintFiles(void)
{
	DIR     dir;
	u8      i=0;

	clearInfoFile();
	
	if(f_opendir(&dir,infoFile.title)!=FR_OK) return false;
	
	for(;;)
	{
		if(f_readdir(&dir,&finfo) !=FR_OK||finfo.fname[0]==0)	      break;       /* 当前目录已经遍历完成 */
		if((finfo.fattrib&AM_HID) != 0)                             continue;    /* 隐藏文件不处理 */
		if(infoFile.f_num>=FILE_NUM && infoFile.F_num>=FOLDER_NUM)  break;       /* 文件夹和文件的数量 均超过最大数量限制 */
		
		len=strlen(finfo.fname)+1;		
		if((finfo.fattrib&AM_DIR) == AM_DIR)									/* 文件夹 */
		{
			if(infoFile.F_num>=FOLDER_NUM)                        continue;   /* 文件夹已超过数量限制 */
			
			infoFile.folder[infoFile.F_num]=malloc(len);
			if(infoFile.folder[infoFile.F_num]==NULL)
				break;
			memcpy(infoFile.folder[infoFile.F_num++],finfo.fname,len);
		
		}
		else                                             /* 文件 */
		{
			if(infoFile.f_num>=FILE_NUM)                          continue; /* Gcode已超过数量限制 */
			
			if(strstr(finfo.fname,".gcode")==NULL)                continue; /* 不是Gcode文件 */
			
			infoFile.file[infoFile.f_num]=malloc(len);
			if(infoFile.file[infoFile.f_num]==NULL)
				break;
			memcpy(infoFile.file[infoFile.f_num++],finfo.fname,len);			
		
		}		
	}
	
	/* 将文件夹倒序排序 */
	for(i=0;i<infoFile.F_num/2;i++)
	{
		char *temp=infoFile.folder[i];		
		infoFile.folder[i]=infoFile.folder[infoFile.F_num-i-1];
		infoFile.folder[infoFile.F_num-i-1]=temp;
	}
	/* 将文件倒序排序 */
	for(i=0;i<infoFile.f_num/2;i++)
	{
		char *temp=infoFile.file[i];		
		infoFile.file[i]=infoFile.file[infoFile.f_num-i-1];
		infoFile.file[infoFile.f_num-i-1]=temp;
	}
	
	return true;
}


/* 
功能:进入下一个目录
返回值:true进入成功 false进入失败
*/
bool EnterDir(char *nextdir)
{
	if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return false;
	strcat(infoFile.title,"/");
	strcat(infoFile.title,nextdir);
	return true;
}


/* 
功能:返回上一层路径
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
功能:当前路径是否为 根目录("0:")
返回值: true是根目录 false不是根目录
*/
bool IsRootDir(void)
{
	return !strchr(infoFile.title,'/');
}


/* 
功能:测试使用,显示文件最近一次更改的时间
*/
void GUI_DispDate(u16 date, u16 time)
{
	char buf[100];
	static u8 i=0;
	sprintf(buf,"%d/%d/%d--%d:%d:%d",1980+(date>>9),(date>>5)&0xF,date&0x1F,time>>11,(time>>5)&0x3F,time&0x1F);
	GUI_DispString(0,i,(u8* )buf,0);
	i+=16;
}


/* 
功能:获取路径path 下最新的Gcode文件
返回值: true已找到最新Gcode文件  false 未找到
*/
bool Get_NewestGcode(const TCHAR* path)
{
	DIR     dirs;
    bool	status=false;	
	
	if (f_opendir(&dirs, path) != FR_OK) return false;
	
	len=strlen(path); 
	while (f_readdir(&dirs, &finfo) == FR_OK) 
	{
		if(finfo.fname[0]==0)                        break;							//已经遍历完 此路径下所有的文件以及文件夹 
		if((finfo.fattrib&AM_HID) != 0)              continue;          //隐藏文件不处理哦
		
		if((finfo.fattrib&AM_DIR) == AM_DIR)                            //文件夹
		{				
            char  *nextdirpath = malloc(len+strlen(finfo.fname)+2);
			if(nextdirpath==NULL)                      break;							//heap内存不足，结束循环
			
			strcpy(nextdirpath, path);
			strcat(nextdirpath,"/");
			strcat(nextdirpath, finfo.fname);	
			
			status|=Get_NewestGcode(nextdirpath);	
			free(nextdirpath);	
			nextdirpath=NULL;
		}
		else
		{
			if(strstr(finfo.fname,".gcode")==NULL)      continue;					//不是Gcode文件不处理
			if(((finfo.fdate <<16)|finfo.ftime) < date)   continue;         //时间比上一个旧的不处理
			
			date=(finfo.fdate <<16)|finfo.ftime;
			resetInfoFile();

			if(len+strlen(finfo.fname)+2>MAX_PATH_LEN)  break;//内存不足，结束循环
			
			strcpy(infoFile.title,path);
			strcat(infoFile.title,"/");
			strcat(infoFile.title,finfo.fname);
			status=true;
		}
	}
  return status;
}
