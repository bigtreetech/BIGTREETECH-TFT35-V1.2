#include "mygcodefs.h"
#include "includes.h"

/* 
*/
char mountGcodeSDCard(void)
{
  // Send M21 ( Init SD )
  // wait result
  // return 0=Error 1=OK
  /*
>>> M21
SENDING:M21
echo:SD card ok
*/
  return request_M21() ? 1 : 0;
}

//static uint32_t date=0;
//static FILINFO  finfo;
//static uint16_t len = 0;
/*
SENDING:M20
Begin file list
PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~3.GCO 11081207
/YEST~1/TEST2/PI3MK2~2.GCO 11081207
/YEST~1/TEST2/PI3MK2~4.GCO 11081207
/YEST~1/TEST2/PI3MK2~5.GCO 11081207
/YEST~1/PI3MK2~1.GCO 11081207
/YEST~1/PI3MK2~3.GCO 11081207
/YEST~1/PI3MK2~2.GCO 11081207
End file list
*/
char scanPrintFilesGcodeFs(void)
{

  clearInfoFile();

  char *data = request_M20();

//  GUI_Clear(BK_COLOR);

//  GUI_DispStringInRect(0,0,LCD_WIDTH,LCD_HEIGHT,(u8 *)data,0);
//  Delay_ms(10000);  
//  GUI_Clear(BK_COLOR);


  const char s[2] = "\n";
  char *line = strtok(data, s);
//  int dbrow = 0;
  for (;line != NULL;line = strtok(NULL, s))
  {
      if( strcmp(line,"Begin file list") == 0 || strcmp(line,"End file list") == 0 || strcmp(line,"ok") == 0)continue; // Start and Stop tag
      if( strlen(line) < strlen(infoFile.title)-4) continue; // No path line exclude
      if(strlen(infoFile.title) > 4 && strstr(line,infoFile.title+4) == NULL) continue; // No current directory

      char *pline = line + strlen(infoFile.title) - 4;
      if(strlen(infoFile.title) > 4)pline++;

/*   dbrow++;
  char * buf = malloc(2000);
  sprintf(buf, "PLine:%s",pline);
  GUI_DispString(0,(BYTE_HEIGHT*dbrow),(u8 *)buf,0);
  free(buf); 
 */   
    if (strchr(pline, '/') == NULL)
    {
      // FILE
      if (infoFile.f_num >= FILE_NUM)
        continue; /* Gcode锟窖筹拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 */

      char* rest = pline;  
      char* file = strtok_r(rest," ",&rest);

      uint16_t len = strlen(file) + 1;

      infoFile.file[infoFile.f_num] = malloc(len);
      if (infoFile.file[infoFile.f_num] == NULL)
        break;
      strcpy(infoFile.file[infoFile.f_num++], file);

  /* dbrow++;
  char * buf = malloc(1000);
  sprintf(buf, "Filename: %s ",infoFile.file[infoFile.f_num-1]);
  GUI_DispString(0,(BYTE_HEIGHT*dbrow),(u8 *)buf,0);
  free(buf); */

    }
    else
    {
      // DIRECTORY
      if (infoFile.F_num >= FOLDER_NUM)
        continue; /* 锟侥硷拷锟斤拷锟窖筹拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 */


      char* rest = pline;  
      char* folder = strtok_r(rest,"/",&rest);

      bool found = false;
      for(int i=0; i < infoFile.F_num; i++)
      {
        if(strcmp(folder, infoFile.folder[i]) == 0)
        {
          found = true;
          break;
        }
      }  

      if(!found)
      {
        uint16_t len = strlen(folder) + 1;
        infoFile.folder[infoFile.F_num] = malloc(len);
        if (infoFile.folder[infoFile.F_num] == NULL)
          break;
        strcpy(infoFile.folder[infoFile.F_num++], folder);
        

  /* dbrow++;
  char * buf = malloc(1000);
  sprintf(buf, "Folder: %s ",infoFile.folder[infoFile.F_num-1]);
  GUI_DispString(0,(BYTE_HEIGHT*dbrow),(u8 *)buf,0);
  free(buf) */;

      }
    }
    /* Delay_ms(1000);   */
  }
  return 1;
}

