#include "photo.h"
#include "includes.h"



u8 Fast_Send_Receive_Byte(u8 data)
{
	SPI1->DR = data;
	return SPI1->DR;
}

void W25QXX_ReadStart(uint32_t ReadAddr)
{
    /*!< Select the FLASH: Chip Select low */
    W25QXX_CS_LOW();

    /*!< Send "Read from Memory " instruction */
    SPI1_Send_Receive_Byte(CMD_FAST_READ_DATA);

    /*!< Send ReadAddr high nibble address byte to read from */
    SPI1_Send_Receive_Byte((ReadAddr & 0xFF0000) >> 16);
    /*!< Send ReadAddr medium nibble address byte to read from */
    SPI1_Send_Receive_Byte((ReadAddr& 0xFF00) >> 8);
    /*!< Send ReadAddr low nibble address byte to read from */
    SPI1_Send_Receive_Byte(ReadAddr & 0xFF);

    Fast_Send_Receive_Byte(FLASH_DUMMY_BYTE);
    Fast_Send_Receive_Byte(FLASH_DUMMY_BYTE);
}

void W25QXX_ReadEnd(void)
{
  W25QXX_CS_HIGH();
}



void Read_Display_Photo(u8 place,u8 rank,u8 flag)
{
	u8 	len=0;
	u16 mode=0xFFFF;
	u16 x,y;
	u16 sx=menuRect[place].x0,
			sy=menuRect[place].y0,
			ex=sx+94,
			ey=sy+94;
	u16 color = 0;
	
	if(flag==1)
	{
		mode=0x0FF0;
	}
	
	LCD_SetWindow(sx,sy,ex,ey);
	W25QXX_ReadStart(ADDRESS(rank));
	for(y=sy;y<=ey;y++)
	{	
		for(x=sx;x<=ex;x++)
		{
			color  = (u8)(Fast_Send_Receive_Byte(FLASH_DUMMY_BYTE))<<8;
			color |= (u8)(Fast_Send_Receive_Byte(FLASH_DUMMY_BYTE));
			LCD_WR_16BITS_DATA(color&mode);
		}
	}		
	W25QXX_ReadEnd();
	
	len=my_strlen(textSelect(rank));
	
	x=sx-12;
	if(len<10)
	{
		x+=((120-12*len)>>1);	
	}
	y=sy+100;

	GUI_DispString(x,y,textSelect(rank),1);
	
}

void Read_Dispaly_Logo(void)
{
	u16 x,y;
	u16 color;
	
	LCD_SetWindow(0,0,479,319);
	W25QXX_ReadStart(0);
	for(y=0;y<=319;y++)
	{	
		for(x=0;x<=479;x++)
		{
			color  = (Fast_Send_Receive_Byte(FLASH_DUMMY_BYTE))<<8;
			color |= (Fast_Send_Receive_Byte(FLASH_DUMMY_BYTE));
			LCD_WR_16BITS_DATA(color);
		}
	}		
	W25QXX_ReadEnd();

}


