#include "GUI_BASIC.h"
#include "lcd.h"
#include "includes.h"
#include "math.h"

#define GUI_LCM_XMAX 480
#define GUI_LCM_YMAX 320

u16 POINT_COLOR = WHITE;
u16 BKG_COLOR	  = BLACK;

void GUI_SetColor(u16 color)
{
	POINT_COLOR=color;
}
void GUI_SetBKColor(u16 color)
{
	BKG_COLOR=color;
}


static RANGE_FONT piexlRange={0,{0,0,479,319}};

void GUI_SetRange(u16 x0, u16 y0, u16 x1, u16 y1)
{
	piexlRange.flag=1;
	piexlRange.range.x0=x0;
	piexlRange.range.x1=x1;
	piexlRange.range.y0=y0;
	piexlRange.range.y1=y1;
}
void GUI_CancelRange(void)
{
	piexlRange.flag=0;
}
void GUI_DrawPixel(u16 x,u16 y,u16 color)
{	   
	if(piexlRange.flag&&(x<piexlRange.range.x0||x>=piexlRange.range.x1||y<piexlRange.range.y0||y>=piexlRange.range.y1))
		return ;

	LCD_SetWindow(x,y,x,y);
	LCD_WR_16BITS_DATA(color);
}
//0xA7A1-0xA7C1  0xA7D1-0xA7F1 66个俄文字符
//0xA140-0xA18B  76个亚美尼亚字符
void GUI_DispChar(u16 sx,u16 sy,u16 ch,u8 mode)
{  		
	u8 		x,y,i=0;
	u8		font[36];
	u32 	offset=0;					//24*12/8 = 36
	u32 	temp;

    if(ch<0xFF)  //ASCII字符
    {
        offset=ch*36;	
    }
    else if(ch>=0xA7A1 && ch<=0xA7F1)  //俄文字符
    {
        offset=(ch-0xA7A1+127)*36;
    }
    else if(ch>=0xA140 && ch<=0xA18B)  //亚美尼亚字符
    {
        offset=(ch-0xA140+208)*36;
    }
	  
	W25QXX_ReadBuffer(font,offset+BYTE_ADDR,36);
	for(x=0;x<12;x++)
	{   
		temp=(font[i++]<<16)|(font[i++]<<8)|font[i++];
		
		for(y=0;y<24;y++)
		{			    
			if(temp&0x800000)GUI_DrawPixel(sx,sy+y,POINT_COLOR);
			else if(mode!=0)GUI_DrawPixel(sx,sy+y,BKG_COLOR);
			temp<<=1;
		} 
		sx++;
	}  	
   	 	  
}

void GUI_DispHz(u16 sx,u16 sy,u8 *hz,u8 mode)
{
	u8 		x,y,i=0;
	u8		font[72];
	u32 	offset=0;					//=hz[0]<<8|hz[1];
	u32 	temp;
	
	
	if(hz[1]<0x7F) 
		offset=(190*(hz[0]-0x81)+hz[1]-0x40)*72;
	else 
		offset=(190*(hz[0]-0x81)+hz[1]-0x41)*72;
	
	W25QXX_ReadBuffer(font,offset+WORD_ADDR,72);
	
	for(x=0;x<24;x++)
	{   
		temp=(font[i++]<<16)|(font[i++]<<8)|font[i++];
		
		for(y=0;y<24;y++)
		{			    
			if(temp&0x800000)GUI_DrawPixel(sx,sy+y,POINT_COLOR);
			else if(mode!=0)GUI_DrawPixel(sx,sy+y,BKG_COLOR);
			temp<<=1;
		} 
		sx++;
	}  	  
}


void GUI_DispString(u16 x,u16 y,u8 *p,u8 mode)
{      
	while(*p!=0)
	{
		if(*p<0x81)
		{			
			GUI_DispChar(x,y,*p,mode);
			x += 12;
			p++;
		}
		else
		{
			if(isRussia(p) || isArmenian(p))
			{
				GUI_DispChar(x, y,(p[0]<<8)|p[1],mode);
				x+=12;
			}
			else
			{
				GUI_DispHz(x,y,(void *)p,mode);
				x+=24;
			}
			p+=2;
		}
	}  
}

void GUI_DispLenString(u16 x,u16 y,u8 *p,u8 mode,u8 len)
{         
	while(*p!=0)
	{
		if(*p<0x81)
		{			
			if(len==0)
				return;
			GUI_DispChar(x,y,*p,mode);
			len--;		
			x+=12;
			p++;

		}
		else
		{
			if(isRussia(p)==true)
			{
				if(len==0)
					return;
				GUI_DispChar(x, y,p[1]-0x22,mode);
				x+=12;
				len--;	
			}
			else
			{
				if(len<2)
					return;
				GUI_DispHz(x,y,(void *)p,mode);
				x+=24;
				len-=2;	
			}
			p+=2;	
		}
	}  
}


void Show_warning(u16 backColor,u16 color,  u8 *chs)
{
	u16 len;
	len=my_strlen(chs);

	len*=12;
	len=160+((312-len)>>1);
	GUI_SetColor(RED);
	GUI_DispString(len,10,chs,1);
	GUI_SetColor(WHITE);
}


void Show_num(u16 x, u16 y,int inum ,u8 len,u8 leftOrRight, u8 mode)
{
	char nstr[20],i=0,j=0;
	u8 str[20];
	int num=inum>0?inum:(-inum);
	
	do
	{
		nstr[i++]=num%10+'0';
		num/=10;
	}while(num&&i<len);
	if(inum<0)
	{
		inum=-inum;
		nstr[i++]='-';
	}
	if(leftOrRight!=0)
	{
		for(;i<len;i++)
			nstr[i]=' ';
	}
	
	for(j=0;j<i;j++)
	{
		str[j]=nstr[i-j-1];
	}

	if(leftOrRight==0)
	{
		for(;j<len;j++)
		{
			str[j]=' ';
		}
	}
	str[len]='\0';
	GUI_DispString(x,y,str,mode);
}

void Show_Clock(u16 x, u16 y,int time)
{
	GUI_DispChar(x,y,time/36000%10+'0',1);
	GUI_DispChar(x+12,y,time/3600%10+'0',1);
	GUI_DispChar(x+12*3,y,time/60%60/10+'0',1);
	GUI_DispChar(x+12*4,y,time/60%10+'0',1);
	GUI_DispChar(x+12*6,y,time%60/10+'0',1);
	GUI_DispChar(x+12*7,y,time%10+'0',1);
}


const u32 GUI_Pow10[10] = {
  1 , 10, 100, 1000, 10000,
  100000, 1000000, 10000000, 100000000, 1000000000
};
void Show_Float(u16 x, u16 y,float num,u8 llen,u8 rlen)
{
    u8		i=0;	
    GUI_DispChar(x+=12,y,num>=0 ? ' ':'-',1);
    num= num>=0 ? num : (-num);
    for(i=0;i<llen;i++)
    {
        GUI_DispChar(x+=12,y,(int)(num/GUI_Pow10[llen-1-i])%10+'0',1);
    }
    GUI_DispChar(x+=12,y,'.',1);

    num *= GUI_Pow10[(unsigned)rlen];
    num += 0.5;
    num = (float) floor (num);
    for(i=0;i<rlen;i++)
    {
        GUI_DispChar(x+=12,y,(int)(num/GUI_Pow10[rlen-1-i])%10+'0',1);
    }
}

void  GUI_HLine(u16 sx, u16 y, u16 ex, u16 color) 
{  
	u16 i;
	for(i=sx;i<=ex;i++)
	{
		LCD_DrawPoint(i,y,color);
	}
}

void  GUI_RLine(u16 x, u16 sy, u16 ey, u16 color) 
{  
	u16 i;
	for(i=sy;i<=ey;i++)
	{
		LCD_DrawPoint(x,i,color);
	}
}

void  GUI_DrawRect(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{  
    GUI_HLine(sx, sy, ex, color);
    GUI_HLine(sx, ey, ex, color);
    GUI_RLine(sx, sy, ey, color);
    GUI_RLine(ex, sy, ey, color);
}

void  GUI_FillRect(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{ 
	u16 x,y;
	LCD_SetWindow(sx,sy,ex,ey);
	for(x=sx;x<=ex;x++)
	{
		for(y=sy;y<=ey;y++)
		{	
			LCD_WR_16BITS_DATA(color);
		}	
	}
}

void  GUI_CircleFill(u16 x0, u16 y0, u8 r,u16 color)
{  int  draw_x0, draw_y0;			// 刽图点坐标变量
   int  draw_x1, draw_y1;	
   int  draw_x2, draw_y2;	
   int  draw_x3, draw_y3;	
   int  draw_x4, draw_y4;	
   int  draw_x5, draw_y5;	
   int  draw_x6, draw_y6;	
   int  draw_x7, draw_y7;	
   int  fill_x0, fill_y0;			// 填充所需的变量，使用垂直线填充
   int  fill_x1;
   int  xx, yy;					// 画圆控制变量
 
   int  di;						// 决策变量
   
   /* 参数过滤 */
   if(0==r) return;
   
   /* 计算出4个特殊点(0、90、180、270度)，进行显示 */
   draw_x0 = draw_x1 = x0;
   draw_y0 = draw_y1 = y0 + r;
   if(draw_y0<GUI_LCM_YMAX)
   {  LCD_DrawPoint(draw_x0, draw_y0, color);	// 90度
   }
    	
   draw_x2 = draw_x3 = x0;
   draw_y2 = draw_y3 = y0 - r;
   if(draw_y2>=0)
   {  LCD_DrawPoint(draw_x2, draw_y2, color);	// 270度
   }
  	
   draw_x4 = draw_x6 = x0 + r;
   draw_y4 = draw_y6 = y0;
   if(draw_x4<GUI_LCM_XMAX) 
   {  LCD_DrawPoint(draw_x4, draw_y4, color);	// 0度
      fill_x1 = draw_x4;
   }
   else
   {  fill_x1 = GUI_LCM_XMAX;
   }
   fill_y0 = y0;							// 设置填充线条起始点fill_x0
   fill_x0 = x0 - r;						// 设置填充线条结束点fill_y1
   if(fill_x0<0) fill_x0 = 0;
   GUI_HLine(fill_x0, fill_y0, fill_x1, color);
   
   draw_x5 = draw_x7 = x0 - r;
   draw_y5 = draw_y7 = y0;
   if(draw_x5>=0) 
   {  LCD_DrawPoint(draw_x5, draw_y5, color);	// 180度
   }
   if(1==r) return;
   
   
   /* 使用Bresenham法进行画圆 */
   di = 3 - 2*r;							// 初始化决策变量
   
   xx = 0;
   yy = r;
   while(xx<yy)
   {  if(di<0)
	  {  di += 4*xx + 6;
	  }
	  else
	  {  di += 4*(xx - yy) + 10;
	  
	     yy--;	  
		 draw_y0--;
		 draw_y1--;
		 draw_y2++;
		 draw_y3++;
		 draw_x4--;
		 draw_x5++;
		 draw_x6--;
		 draw_x7++;		 
	  }
	  
	  xx++;   
	  draw_x0++;
	  draw_x1--;
	  draw_x2++;
	  draw_x3--;
	  draw_y4++;
	  draw_y5++;
	  draw_y6--;
	  draw_y7--;
		
	
	  /* 要判断当前点是否在有效范围内 */
	  if( (draw_x0<=GUI_LCM_XMAX)&&(draw_y0>=0) )	
	  {  LCD_DrawPoint(draw_x0, draw_y0, color);
	  }	    
	  if( (draw_x1>=0)&&(draw_y1>=0) )	
	  {  LCD_DrawPoint(draw_x1, draw_y1, color);
	  }
	  
	  /* 第二点水直线填充(下半圆的点) */
	  if(draw_x1>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x1;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y1;
         if(fill_y0>GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;
         if(fill_y0<0) fill_y0 = 0; 
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x1;				
         if(fill_x1>GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;
         GUI_HLine(fill_x0, fill_y0, fill_x1, color);
      }
	  
	  
	  if( (draw_x2<=GUI_LCM_XMAX)&&(draw_y2<=GUI_LCM_YMAX) )	
	  {  LCD_DrawPoint(draw_x2, draw_y2, color);   
	  }
	    	  
	  if( (draw_x3>=0)&&(draw_y3<=GUI_LCM_YMAX) )	
	  {  LCD_DrawPoint(draw_x3, draw_y3, color);
	  }
	  
	  /* 第四点垂直线填充(上半圆的点) */
	  if(draw_x3>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x3;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y3;
         if(fill_y0>GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;
         if(fill_y0<0) fill_y0 = 0;
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x3;				
         if(fill_x1>GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;
         GUI_HLine(fill_x0, fill_y0, fill_x1, color);
      }
	  
	  	  
	  if( (draw_x4<=GUI_LCM_XMAX)&&(draw_y4>=0) )	
	  {  LCD_DrawPoint(draw_x4, draw_y4, color);
	  }
	  if( (draw_x5>=0)&&(draw_y5>=0) )	
	  {  LCD_DrawPoint(draw_x5, draw_y5, color);
	  }
	  
	  /* 第六点垂直线填充(上半圆的点) */
	  if(draw_x5>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x5;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y5;
         if(fill_y0>GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;
         if(fill_y0<0) fill_y0 = 0;
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x5;				
         if(fill_x1>GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;
         GUI_HLine(fill_x0, fill_y0, fill_x1, color);
      }
	  
	  
	  if( (draw_x6<=GUI_LCM_XMAX)&&(draw_y6<=GUI_LCM_YMAX) )	
	  {  LCD_DrawPoint(draw_x6, draw_y6, color);
	  }
	  
	  if( (draw_x7>=0)&&(draw_y7<=GUI_LCM_YMAX) )	
	  {  LCD_DrawPoint(draw_x7, draw_y7, color);
	  }
	  
	  /* 第八点垂直线填充(上半圆的点) */
	  if(draw_x7>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x7;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y7;
         if(fill_y0>GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;
         if(fill_y0<0) fill_y0 = 0;
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x7;				
         if(fill_x1>GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;
         GUI_HLine(fill_x0, fill_y0, fill_x1, color);
      }
	  
   }
}
