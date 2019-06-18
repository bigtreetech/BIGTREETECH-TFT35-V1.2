#include "ui_draw.h"
#include "includes.h"

#if   HARDWARE_VERSION == V1_2
  #define IS_16BIT 1
#elif HARDWARE_VERSION == V1_1 ||  HARDWARE_VERSION == V1_0
  #define IS_16BIT 0
#endif

//SPI2 --> LCD_RAM dma配置
//16位,SPI1_RX传输到LCD_RAM.
void LCD_DMA_Config(void)
{  
  RCC->AHBENR|=1<<0;				//开启DMA1时钟
  Delay_ms(5);					//等待DMA时钟稳定
  DMA1_Channel2->CPAR=(u32)&SPI1->DR;			//外设地址为:SPI1->DR
  DMA1_Channel2->CMAR=(u32)&LCD->LCD_RAM;//目标地址为LCD_RAM
  DMA1_Channel2->CNDTR=0;			//DMA1,传输数据量,暂时设置为0
  DMA1_Channel2->CCR=0X00000000;	//复位
  DMA1_Channel2->CCR|=0<<4;		//从外设读
  DMA1_Channel2->CCR|=0<<5;		//普通模式
  DMA1_Channel2->CCR|=0<<6;		//外设地址非增量模式
  DMA1_Channel2->CCR|=0<<7;		//存储器非增量模式
  DMA1_Channel2->CCR|=IS_16BIT<<8;		//外设数据宽度为16位
  DMA1_Channel2->CCR|=IS_16BIT<<10;		//存储器数据宽度16位
  DMA1_Channel2->CCR|=1<<12;		//中等优先级
  DMA1_Channel2->CCR|=0<<14;		//非存储器到存储器模式
} 

//开启一次SPI到LCD的DMA的传输 
#define LCD_DMA_MAX_TRANS	65535		//DMA一次最多传输 65535 字节	

// 启动 SPI->DR 到 FSMC 的 DMA 传输 
// 传输最大个数 不能超过 LCD_DMA_MAX_TRANS 65535
void lcd_frame_segment_display(u16 size, u32 addr)
{     
  DMA1_Channel2->CNDTR = size;				    //设置传输长度

  W25Qxx_SPI_CS_Set(0);                        //使能器件 
  W25Qxx_SPI_Read_Write_Byte(CMD_FAST_READ_DATA);	//发送快速读取命令   
  W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>16));	//发送24bit地址    
  W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>8));   
  W25Qxx_SPI_Read_Write_Byte((u8)addr);  
  W25Qxx_SPI_Read_Write_Byte(0XFF);	           //8 dummy clock  

  //设置 SPI 到 16bit DMA 接收模式    
  SPI1->CR1 &= ~(1<<6); 	
  SPI1->CR2 |= 1<<0;		    //使用DMA接收 
  SPI1->CR1 |= IS_16BIT<<11;	//16bit数据格式	
  SPI1->CR1 |= 1<<10; 		//仅接收模式  

  DMA1_Channel2->CCR |= 1<<0;				//通道开启
  SPI1->CR1 |= 1<<6;                      //开启此次传输

  while((DMA1->ISR&(1<<5)) == 0);			//等待传输完成  
  DMA1_Channel2->CCR &= (u32)(~(1<<0));	//关闭通道
  DMA1->IFCR |= (u32)(1<<5);				//清除上次的传输完成标记 
  W25Qxx_SPI_CS_Set(1) ;  	 	 		//禁止W25QXX

  //设置 SPI 到普通模式    
  RCC->APB2RSTR |= 1<<12;     //复位SPI1
  RCC->APB2RSTR &= ~(1<<12);
  SPI1->CR1 = 0x34C;          //设置CR1  
  SPI1->I2SCFGR &= ~(1<<11);  //选择SPI模式 
}

void lcd_frame_display(u16 sx,u16 sy,u16 w,u16 h, u32 addr)
{
  u32 cur=0;
  u32 segmentSize;
  u32 totalSize = w*h*(2-IS_16BIT);

  LCD_SetWindow(sx,sy,sx+w-1,sy+h-1);
  LCD->LCD_REG = 0x2C;    

  for(cur = 0; cur < totalSize; cur += LCD_DMA_MAX_TRANS)
  {
    segmentSize = cur+LCD_DMA_MAX_TRANS<=totalSize ? LCD_DMA_MAX_TRANS : totalSize-cur;
    lcd_frame_segment_display(segmentSize, addr+cur*(IS_16BIT + 1));
  }
}

void LOGO_ReadDisplay(void)
{
  lcd_frame_display(0, 0, LCD_WIDTH, LCD_HEIGHT, LOGO_ADDR);
}

void ICON_ReadDisplay(u16 sx,u16 sy, u8 icon)
{
  lcd_frame_display(sx, sy, ICON_WIDTH, ICON_HEIGHT, ICON_ADDR(icon));
}

void ICON_PressedDisplay(u16 sx,u16 sy, u8 icon)
{
  u16 mode=0x0FF0;
  u16 x,y;
  u16 color = 0;
  u32 address = ICON_ADDR(icon);

  LCD_SetWindow(sx, sy, sx+ICON_WIDTH-1, sy+ICON_HEIGHT-1);
  LCD->LCD_REG=0x2C;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
  W25Qxx_SPI_Read_Write_Byte((address& 0xFF00) >> 8);
  W25Qxx_SPI_Read_Write_Byte(address & 0xFF);

  for(y=sy; y<sy+ICON_WIDTH; y++)
  {	
    for(x=sx; x<sx+ICON_HEIGHT; x++)
    {
      color  = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE)<<8);
      color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
      LCD_WR_16BITS_DATA(color & mode);
    }
  }		
  W25Qxx_SPI_CS_Set(1);		
}
