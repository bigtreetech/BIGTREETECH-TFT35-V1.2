#ifndef _TOUCH_H_
#define _TOUCH_H_
#include "stm32f10x.h"
/*
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
*/

#define TPEN() 	((GPIOC->IDR & GPIO_Pin_4)!=Bit_RESET)
#define TDIN() ((GPIOC->IDR & GPIO_Pin_2)!=Bit_RESET)

#define TCS(x)  do{if(x)(GPIOC->BSRR = GPIO_Pin_0);else (GPIOC->BRR = GPIO_Pin_0);}while(0)
#define TCLK(x) do{if(x)(GPIOC->BSRR = GPIO_Pin_1);else GPIO_ResetBits(GPIOC,GPIO_Pin_1);}while(0)
#define TOUT(x) do{if(x)(GPIOC->BSRR = GPIO_Pin_3);else GPIO_ResetBits(GPIOC,GPIO_Pin_3);}while(0)

#define CMD_RDX 0xD0
#define CMD_RDY 0x90
extern u32 A,B,C,D,E,F,K;

#define ADJUST_IDADDR  0x0803F000	///校准标示 及 值存储位置
#define ADJUST_COMPLETE   0x12345678	///芯片ID存储点原始密码

/*******************************************************************************
功能描述：读取STM32flash指定地址的内容
输入参数；u32 faddr：地址
输出参数；地址对应的u32内容
备    注: 无
*************************************************************************/
u32 STMFLASH_ReadHalfWord(u32 faddr);

/*******************************************************************************
功能描述：存储三点校准值到FLASH
输入参数；u32 faddr：地址
输出参数；void
备    注: 无
*************************************************************************/
void STMFLASH_Store_Cost(u32 faddr);

/*******************************************************************************
功能描述：读取保存的 A B C D E F K的值
输入参数；u32 faddr：地址
输出参数；void
备    注: 无
*************************************************************************/
void STMFLASH_Read_Calibration(void);

void Read_Lcd_xy(void);

void TP_GPIOConfig(void);
void TP_Write_Cmd(u8 CMD);
void TP_Read_Data(u16 *AD_Data);
u16 TP_Read_AD(u8 CMD);
void TP_Read_xy(u16 *x,u16 *y);
void Test_touch(void);
void Three_adjust(void);
void Adjust_test(u16 x,u16 y);
//void Display_LCD_xy(void);

void Get_Lcdvalue(u16 *lcd_x,u16 *lcd_y);

u8 Get_Place(u8 view_flag);
u8 Key_value(u16 x,u16 y);

void Back_display(u8 press,u8 view_flag,u8 key_num);

//void Key_PicAndNum_create(void);
//void Key_PicAndNum_alone(u8 i,u8 j,u16 linec,u16 fullc,u16 zic);
#endif

