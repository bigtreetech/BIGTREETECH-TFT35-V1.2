#ifndef _SD_H_
#define _SD_H_	 
#include "stm32f10x.h"
#include "spi.h"
/******************************************************************* 				
本程序使用的硬件接口：
SD卡采用SPI接口通讯

DATA1--PC9

PG6
DATA2----------SD_DT2
CD/DATA3-------SD_DT3
CMD------------SD_CMD
//SPI接口
CS--------------PD2(片选线)
CLK----SD_SCK---PB13(时钟线)
DATA0--SD_DT0---PB14(输出)
DATA1--SD_DT1---PB15(输入)
********************************************************************/
				
// SD卡类型定义  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	   

// SD卡指令表  	   
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置扇区大小 应返回0x00
#define CMD17   17      //命令17，读扇区
#define CMD18   18      //命令18，读Multi 扇区
#define CMD23   23      //命令23，设置多扇区写入前预先擦除N个block
#define CMD24   24      //命令24，写扇区
#define CMD25   25      //命令25，写多个扇区
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00、

/*数据写入回应字意义*/
#define SD_DATA_OK                0x05
#define SD_DATA_CRC_ERROR         0x0B
#define SD_DATA_WRITE_ERROR       0x0D
#define SD_DATA_OTHER_ERROR       0xFF

/*SD卡回应标记字*/
#define SD_RESPONSE_NO_ERROR      0x00   //响应错误
#define SD_IN_IDLE_STATE          0x01   //闲置状态
#define SD_ERASE_RESET            0x02   //擦除复位
#define SD_ILLEGAL_COMMAND        0x04
#define SD_COM_CRC_ERROR          0x08
#define SD_ERASE_SEQUENCE_ERROR   0x10
#define SD_ADDRESS_ERROR          0x20
#define SD_PARAMETER_ERROR        0x40
#define SD_RESPONSE_FAILURE       0xFF
 							   						 					    	  
#define SD_CS_H()	GPIOB->BSRR |=GPIO_Pin_12
#define SD_CS_L()	GPIOB->BRR  |=GPIO_Pin_12

extern u8  SD_Type; //SD卡的类型

//函数申明区 
u8 SD_SPI_Read_Write_Byte(u8 data);               //底层接口，SPI读写字节函数
u8 SD_Wait_Ready(void);							           		//等待SD卡准备
u8 SD_Get_Ack(u8 Response);					       				//获得应答
u8 SD_Init(void);							            				//初始化
u8 SD_Read_Data(u8*buf,u32 sector,u32 cnt);		    //读块
u8 SD_Write_Data(u8*buf,u32 sector,u32 cnt);		  //写块
u32 SD_Get_Sector_Count(void);   					        //读扇区数
u8 SD_GetCID(u8 *cid_data);                       //读SD卡CID
u8 SD_GetCSD(u8 *csd_data);                       //读SD卡CSD
void SD_SPI_Init(void);		//初始化IO
#endif


