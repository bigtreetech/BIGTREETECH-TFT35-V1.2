#include "myfatfs.h"
#include "includes.h"

FATFS getFileFs; /* �ļ�ϵͳ����, ȷ���˱������ڴ�һֱ���� */

MYFILE  infoFile={"SD:", {0}, {0}, 0, 0, 0};

/* 
����:����SD��
����ֵ: true�ɹ� falseʧ��
*/
char mountSDCard(void)
{
  if(f_mount(&getFileFs,"SD:",1) != FR_OK)  /* �����ļ�ϵͳ */
  {
    return 0;
  }	
  return 1;
}


/* 
����:���infoFile �ͷ�malloc������ڴ棬�����ڴ�й©
*/
void clearInfoFile(void)
{	
  uint8_t i=0;
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
����:��ʼ��infoFile
*/
void resetInfoFile(void)
{
  clearInfoFile();
  memset(&infoFile,0,sizeof(infoFile));
  memcpy(infoFile.title,"SD:",4);
}


static uint32_t date=0; 
static FILINFO  finfo;
static uint16_t len = 0;
/* 
����:ɨ�赱ǰ·���µ� �ɴ�ӡ�ļ�
����ֵ: trueɨ��ɹ� falseɨ��ʧ��
*/
char scanPrintFiles(void)
{
  DIR     dir;
  uint8_t i=0;

  clearInfoFile();

  if(f_opendir(&dir,infoFile.title)!=FR_OK) return 0;

  for(;;)
  {
    if(f_readdir(&dir,&finfo) !=FR_OK||finfo.fname[0]==0)       break;       /* ��ǰĿ¼�Ѿ�������� */
    if((finfo.fattrib&AM_HID) != 0)                             continue;    /* �����ļ������� */
    if(infoFile.f_num>=FILE_NUM && infoFile.F_num>=FOLDER_NUM)  break;       /* �ļ��к��ļ������� ����������������� */
    
    len=strlen(finfo.fname)+1;
    if((finfo.fattrib&AM_DIR) == AM_DIR)      /* �ļ��� */
    {
      if(infoFile.F_num>=FOLDER_NUM)                            continue;   /* �ļ����ѳ����������� */

      infoFile.folder[infoFile.F_num]=malloc(len);
      if(infoFile.folder[infoFile.F_num]==NULL)
      break;
      memcpy(infoFile.folder[infoFile.F_num++],finfo.fname,len);
    }
    else                                             /* �ļ� */
    {
      if(infoFile.f_num>=FILE_NUM)                              continue; /* Gcode�ѳ����������� */\
      
      if(strstr(finfo.fname,".gcode")==NULL)                    continue; /* ����Gcode�ļ� */

      infoFile.file[infoFile.f_num]=malloc(len);
      if(infoFile.file[infoFile.f_num]==NULL)                   break;
      memcpy(infoFile.file[infoFile.f_num++],finfo.fname,len);
    }		
  }

  /* ���ļ��е������� */
  for(i=0;i<infoFile.F_num/2;i++)
  {
    char *temp=infoFile.folder[i];		
    infoFile.folder[i]=infoFile.folder[infoFile.F_num-i-1];
    infoFile.folder[infoFile.F_num-i-1]=temp;
  }
  /* ���ļ��������� */
  for(i=0;i<infoFile.f_num/2;i++)
  {
    char *temp=infoFile.file[i];		
    infoFile.file[i]=infoFile.file[infoFile.f_num-i-1];
    infoFile.file[infoFile.f_num-i-1]=temp;
  }
  return 1;
}


/* 
����:������һ��Ŀ¼
����ֵ:true����ɹ� false����ʧ��
*/
char EnterDir(char *nextdir)
{
  if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return 0;
  strcat(infoFile.title,"/");
  strcat(infoFile.title,nextdir);
  return 1;
}


/* 
����:������һ��·��
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
����:��ǰ·���Ƿ�Ϊ ��Ŀ¼("0:")
����ֵ: true�Ǹ�Ŀ¼ false���Ǹ�Ŀ¼
*/
char IsRootDir(void)
{
  return !strchr(infoFile.title,'/');
}


/* 
����:����ʹ��,��ʾ�ļ����һ�θ��ĵ�ʱ��
*/
void GUI_DispDate(uint16_t date, uint16_t time)
{
  char buf[100];
  static uint8_t i=0;
  sprintf(buf,"%d/%d/%d--%d:%d:%d",1980+(date>>9),(date>>5)&0xF,date&0x1F,time>>11,(time>>5)&0x3F,time&0x1F);
  GUI_DispString(0,i,(uint8_t* )buf,0);
  i+=16;
}


/* 
����:��ȡ·��path �����µ�Gcode�ļ�
����ֵ: true���ҵ�����Gcode�ļ�  false δ�ҵ�
*/
char Get_NewestGcode(const TCHAR* path)
{
  DIR     dirs;
  char	status = 0;	

  if (f_opendir(&dirs, path) != FR_OK) return 0;

  len=strlen(path); 
  while (f_readdir(&dirs, &finfo) == FR_OK) 
  {
    if(finfo.fname[0]==0)                           break;							//�Ѿ������� ��·�������е��ļ��Լ��ļ��� 
    if((finfo.fattrib&AM_HID) != 0)                 continue;          //�����ļ�������Ŷ

    if((finfo.fattrib&AM_DIR) == AM_DIR)                            //�ļ���
    {				
      char  *nextdirpath = malloc(len+strlen(finfo.fname)+2);
      if(nextdirpath==NULL)                         break;							//heap�ڴ治�㣬����ѭ��

      strcpy(nextdirpath, path);
      strcat(nextdirpath,"/");
      strcat(nextdirpath, finfo.fname);	

      status|=Get_NewestGcode(nextdirpath);	
      free(nextdirpath);	
      nextdirpath=NULL;
    }
    else
    {
      if(strstr(finfo.fname,".gcode")==NULL)        continue;					//����Gcode�ļ�������
      if(((finfo.fdate <<16)|finfo.ftime) < date)   continue;         //ʱ�����һ���ɵĲ�����

      date=(finfo.fdate <<16)|finfo.ftime;
      resetInfoFile();

      if(len+strlen(finfo.fname)+2>MAX_PATH_LEN)    break;//�ڴ治�㣬����ѭ��

      strcpy(infoFile.title,path);
      strcat(infoFile.title,"/");
      strcat(infoFile.title,finfo.fname);
      status = 1;
    }
  }
  return status;
}
