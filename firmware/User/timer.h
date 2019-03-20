#ifndef __TIMER_H_
#define __TIMER_H_
#include "stm32f10x.h"
 	  

typedef void (*FP_TASK)(void *); 

typedef struct 
{     
  u32     time;
  u32     start_time;
  FP_TASK task;
  void *  para;
  char    is_exist:1; 
  char    is_repeat:1; 
}TASK_T;



void TIM4_Config(u16 psc,u16 arr); 
u32 OS_GetTime(void);


void os_task_init(TASK_T *task_t,u32 time,FP_TASK task,void *para);
void os_task_check(TASK_T *task_t);
void os_task_enable(TASK_T *task_t,u8 is_exec,u8 is_repeat);
void os_task_disable(TASK_T *task_t);
 
#endif
