#ifndef _PRINTING_H_
#define _PRINTING_H_
#include "stm32f10x.h"
#include "stdbool.h"
#include "ff.h"

typedef struct 
{	
  FIL     file;

  u32     time;           //打印时间  单位：秒
  u32     size;           //待打印文件的总字节数
  u32     cur;            //已读取过的字节数
  u8      progress;
  bool    printing;      //打印标志  空闲、准备中、打印中、打印完成
  bool    pause;          //1：暂停  0：未暂停
}PRINTING;



void exitPrinting(void);
void endPrinting(void);
void completePrinting(void);
void haltPrinting(void);

bool setPrintPause(bool is_pause);

bool isPrinting(void);	
bool isPause(void);
void setPrintingTime(u32 RTtime);

void setPrintSize(u32 size);
void setPrintCur(u32 cur);

u8   getPrintProgress(void);
u32  getPrintTime(void);

void startGcodeExecute(void);
void endGcodeExecute(void);

void getGcodeFromFile(void);

void menuBeforePrinting(void);
void menuPrinting(void);
void menuStopPrinting(void);
#endif
