#include "W25Qxx.h"
#include "includes.h"

#define W25QXX_TINY		1


void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	//MOSI -- PA7 SCK -- PA5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	W25QXX_CS_HIGH();

	
	//SPI协议的初始化
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);
	W25QXX_CS_LOW();

}



u8 SPI1_Send_Receive_Byte(u8 data)
{
	while((SPI1->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);
	 SPI1->DR = data;
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
	return (SPI1->DR);
}


#if	W25QXX_TINY==0
/****************************************************
函数名：W25QXX_WriteEnable
形参：
返回值：
函数功能：写使能
****************************************************/
void W25QXX_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "Write Enable" instruction */
  SPI1_Send_Receive_Byte(CMD_WRITE_ENABLE);

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();
}
/****************************************************
函数名：W25QXX_WaitForWriteEnd
形参：  
返回值：
函数功能：判断FLASH是否BUSY
****************************************************/
void W25QXX_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  SPI1_Send_Receive_Byte(CMD_READ_REGISTER1);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  }
  while ((flashstatus & 0x01) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();
}
/*
往FLASH中写数据（写一次的数据大小为小于256）
1.判断FLASH是否处于忙状态 1 忙  0 不忙
2.写使能
3.片选CS拉低
4.发送写命令
5.发送24位地址
6.往SPI中写小于4K长度的数据
7.CS拉高
8.判断FLASH状态是否忙
*/
/****************************************************
函数名：W25QXX_WritePage
形参：  pBuffer -- 要存放的数
        WriteAddr -- 存数的地址
        NumByteToWrite -- 存数的个数
返回值：
函数功能：FLASH的页写功能
****************************************************/
void W25QXX_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  W25QXX_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  SPI1_Send_Receive_Byte(CMD_PAGE_PROGRAM);
  /*!< Send WriteAddr high nibble address byte to write to */
  SPI1_Send_Receive_Byte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  SPI1_Send_Receive_Byte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  SPI1_Send_Receive_Byte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    SPI1_Send_Receive_Byte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }
//	DMA1_Star_SPI1_TX(pBuffer,NumByteToWrite);
	
	
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

//向FLASH中发送buffer的数据
/****************************************************
函数名：W25QXX_WriteBuffer
形参：  pBuffer -- 要存放的数
        WriteAddr -- 存数的地址
        NumByteToWrite -- 存数的个数
返回值：
函数功能：往FLASH写数据
****************************************************/
void W25QXX_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % FLASH_SPI_PAGESIZE;//代表从一页中哪个地址开始写数据
  count = FLASH_SPI_PAGESIZE - Addr;//代表该页还可以写多少数据
  NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;//代表要存入的数据长度可以写多少页
  NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;//代表除了整页数据外，还可以写的数据长度

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      W25QXX_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while(NumOfPage--)
      {
        W25QXX_WritePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }
      W25QXX_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        W25QXX_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        W25QXX_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        W25QXX_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

      W25QXX_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        W25QXX_WritePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        W25QXX_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

//读ID
/****************************************************
函数名：W25QXX_ReadID
形参：
返回值：返回读到的ID号
函数功能：读取FLASH的ID号
****************************************************/
uint32_t W25QXX_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "RDID " instruction */
  SPI1_Send_Receive_Byte(CMD_READ_ID);

  /*!< Read a byte from the FLASH */
  Temp0 = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/****************************************************
函数名：W25QXX_EraseSector
形参：  SectorAddr扇区地址
返回值：
函数功能：扇区擦除
****************************************************/
void W25QXX_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  W25QXX_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send Sector Erase instruction */
  SPI1_Send_Receive_Byte(CMD_SECTOR_ERASE);
  /*!< Send SectorAddr high nibble address byte */
  SPI1_Send_Receive_Byte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  SPI1_Send_Receive_Byte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  SPI1_Send_Receive_Byte(SectorAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

void W25QXX_EraseBlock(uint32_t BlockAddr)
{
  /*!< Send write enable instruction */
  W25QXX_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send Sector Erase instruction */
  SPI1_Send_Receive_Byte(CMD_BLOCK_ERASE);
  /*!< Send SectorAddr high nibble address byte */
  SPI1_Send_Receive_Byte((BlockAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  SPI1_Send_Receive_Byte((BlockAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  SPI1_Send_Receive_Byte(BlockAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

void W25QXX_EraseBulk(void)
{
  /*!< Send write enable instruction */
  W25QXX_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  SPI1_Send_Receive_Byte(CMD_FLASH__BE);
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

#endif

/****************************************************
函数名：W25QXX_ReadBuffer
形参：  pBuffer -- 读取的数据保存的数组
        ReadAddr -- 读取地址
        NumByteToRead -- 读取参数的个数
返回值：
函数功能：从FLASH中读数据
****************************************************/
void W25QXX_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  SPI1_Send_Receive_Byte(CMD_READ_DATA);

  /*!< Send ReadAddr high nibble address byte to read from */
  SPI1_Send_Receive_Byte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  SPI1_Send_Receive_Byte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  SPI1_Send_Receive_Byte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }
//	DMA1_Star_SPI1_RX(pBuffer,NumByteToRead);

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();
}
