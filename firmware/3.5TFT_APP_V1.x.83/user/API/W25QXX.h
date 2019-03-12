#ifndef _FLASH_H_
#define _FLASH_H_
#include "stm32f10x.h"


#define CMD_WRITE_ENABLE   0x06
#define CMD_WRITE_DISABLE  0x04
#define CMD_READ_REGISTER1 0x05
#define CMD_READ_REGISTER2 0x35
#define CMD_PAGE_PROGRAM   0x02
#define CMD_SECTOR_ERASE   0x20
#define CMD_BLOCK_ERASE    0xD8
#define CMD_FLASH__BE      0xC7

#define CMD_READ_ID        0x9F
#define CMD_READ_DATA      0x03
#define CMD_FAST_READ_DATA      0x0B

#define Dummy_Byte         0xA5

#define W25QXX_CS_LOW()       GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define W25QXX_CS_HIGH()      GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define FLASH_W25Q64_ID   0xEF4017
#define FLASH_DUMMY_BYTE  0xFF

#define FLASH_SPI_PAGESIZE 0x100


u8 SPI1_Send_Receive_Byte(u8 data);
void W25QXX_Init(void);
void W25QXX_WriteEnable(void);
void W25QXX_WaitForWriteEnd(void);
void W25QXX_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25QXX_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25QXX_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t W25QXX_ReadID(void);
void W25QXX_EraseSector(uint32_t SectorAddr);
void Clear_buffer(u8 *buffer);
void Display_buffer(u8 *buffer);

void Test_Flash(void);
void DMA1_Config(void);
void DMA_Start(u16 lenth);

void W25QXX_EraseBlock(uint32_t BlockAddr);
void W25QXX_EraseBulk(void);

#endif 


