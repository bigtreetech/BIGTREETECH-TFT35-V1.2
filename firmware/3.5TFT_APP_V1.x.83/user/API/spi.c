#include "spi.h"
#include "includes.h"
 
//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25Q64/NRF24L01						  
//SPI口初始化
//这里针是对SPI2的初始化


SPI_InitTypeDef  SPI_InitStructure;
void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
   /* Enable the SPI periph */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
   
   /* Enable SCK, MOSI, MISO and NSS GPIO clocks */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); 

	 
   /* SPI configuration -------------------------------------------------------*/

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	
//	SPI2_ReadWriteByte(0xff);
}   
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler;
	SPI_Init(SPI2,&SPI_InitStructure);
	SPI_Cmd(SPI2,ENABLE); 

} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				

	while (((SPI2->SR)&SPI_I2S_FLAG_TXE)== RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI2->DR = (u16)TxData; //通过外设SPIx发送一个数据
	retry=0;

	while (((SPI2->SR)&SPI_I2S_FLAG_RXNE)== RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return (u8)(SPI2->DR); //返回通过SPIx最近接收的数据					    
}


//void DMA1_Init(void)
//{
//	DMA_InitTypeDef DMA_InitStructure;
//	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
//	

////	
//	//SPI1 DMA
//	DMA_DeInit(DMA1_Channel3);
//	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;
//	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize=512;
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel3,&DMA_InitStructure);
//	
//	DMA_DeInit(DMA1_Channel2);
//	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;
//	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
//	DMA_InitStructure.DMA_BufferSize=512;
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel2,&DMA_InitStructure);
//	
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
//}


//void DMA1_Star_SPI1_RX(u8 *buffer,u16 len)
//{
//    u8 temp=0xff;
//    	
//		DMA1_Channel2->CNDTR=len; 
//		DMA1_Channel2->CMAR=(uint32_t)buffer; 
//		

//		DMA1_Channel3->CNDTR=len; 
//		DMA1_Channel3->CMAR=(uint32_t)&temp; 
//		DMA1_Channel3->CCR&=~DMA_MemoryInc_Enable; 
//		
//		DMA_Cmd(DMA1_Channel2,ENABLE); 
//		DMA_Cmd(DMA1_Channel3,ENABLE); 
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC2)); 
//		DMA_ClearFlag(DMA1_FLAG_TC2); 
//		DMA_ClearFlag(DMA1_FLAG_TC3);
//		DMA_Cmd(DMA1_Channel2,DISABLE);
//		DMA_Cmd(DMA1_Channel3,DISABLE); 
//		
//		DMA1_Channel3->CCR|=DMA_MemoryInc_Enable;

//}

////向sd卡写入一个数据包的内容 512字节 启动DMA传输
////buf:数据缓存区
//void DMA1_Star_SPI1_TX(u8 *buffer,u16 len)
//{
//		DMA1_Channel3->CNDTR=len; //设置要传输的数据长度
//		DMA1_Channel3->CMAR=(uint32_t)buffer; //设置RAM缓冲区地址
//		
//		DMA_Cmd(DMA1_Channel3,ENABLE); //启动DMA传输 RAM->SPI
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC3)); //等待DMA通道5传输完成
//		DMA_ClearFlag(DMA1_FLAG_TC3); //清除通道5传输完成状态标记
//		DMA_Cmd(DMA1_Channel3,DISABLE); //使DMA通道5停止工作
//}


























