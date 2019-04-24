#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f10x_conf.h"

typedef enum
{
  _SPI1 = 0,
  _SPI2,
  _SPI3,
  _SPI_CNT
}_SPI_ENUM;


void SPI_Config(_SPI_ENUM port);
void SPI_Protocol_Init(_SPI_ENUM port, uint8_t baudrate);
uint16_t SPI_Read_Write(_SPI_ENUM port, uint16_t d);
void SPI_CS_Set(_SPI_ENUM port, uint8_t level);

#endif
