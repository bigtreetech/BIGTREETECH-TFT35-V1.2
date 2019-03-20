#include "GUI.h"
#include "includes.h"



u16 COLOR=FK_COLOR;
u16 BKCOLOR=BK_COLOR;

	 
void LCD_SetWindow(u16 sx, u16 sy, u16 ex, u16 ey)
{
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
	LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
	LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
}

void GUI_SetColor(u16 color)
{
	COLOR=color;
}
void GUI_SetBkColor(u16 bkcolor)
{
	BKCOLOR=bkcolor;
}
u16 GUI_GetColor(void)
{
	return COLOR;
}
u16 GUI_GetBkColor(void)
{
	return BKCOLOR;
}
void GUI_Clear(u16 color)
{
    u32 index=0;    
    LCD_SetWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
	LCD_WR_REG(0x2C);  
	for(index=0;index<LCD_WIDTH*LCD_HEIGHT;index++)
	{
		LCD_WR_16BITS_DATA(color);
//		Delay_ms(1);
	}
}  
typedef struct
{
	u8 flag;
	GUI_RECT range;
}RANGE_FONT;

static RANGE_FONT piexlRange;

void GUI_SetRange(s16 x0, s16 y0, s16 x1, s16 y1)
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

void GUI_DrawPixel(s16 x,s16 y,u16 color)
{	   
	if(piexlRange.flag 
       &&(x < piexlRange.range.x0
        ||x >= piexlRange.range.x1
        ||y < piexlRange.range.y0
        ||y >= piexlRange.range.y1))
		return ;
	LCD_SetWindow(x,y,x,y);		 	 
	LCD_WR_REG(0x2C);
	LCD_WR_16BITS_DATA(color);	
}
void GUI_DrawPoint(u16 x,u16 y)
{	   
	LCD_SetWindow(x,y,x,y);			 	 
	LCD_WR_REG(0x2C);
	LCD_WR_16BITS_DATA(COLOR);	
}
void GUI_FillRect(u16 sx,u16 sy,u16 ex,u16 ey)
{
	u16 i=0,j=0;
	LCD_SetWindow( sx, sy, ex-1, ey-1);
	LCD_WR_REG(0x2C);
	for(i=sx;i<ex;i++)
	{
		for(j=sy;j<ey;j++)
		{
			LCD_WR_16BITS_DATA(COLOR);
		}
	}
}
void GUI_ClearRect(u16 sx,u16 sy,u16 ex,u16 ey)
{
	u16 i=0,j=0;
	LCD_SetWindow( sx, sy, ex-1, ey-1);
	LCD_WR_REG(0x2C);
	for(i=sx;i<ex;i++)
	{
		for(j=sy;j<ey;j++)
		{
			LCD_WR_16BITS_DATA(BKCOLOR);
		}
	}
}
void GUI_FillRectColor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{
	u16 i=0,j=0;
	LCD_SetWindow( sx, sy, ex-1, ey-1);
	LCD_WR_REG(0x2C);
	for(i=sx;i<ex;i++)
	{
		for(j=sy;j<ey;j++)
		{
			LCD_WR_16BITS_DATA(color);
		}
	}
}
void GUI_FillRectArry(u16 sx,u16 sy,u16 ex,u16 ey,u8 *arry)
{
	u16 i=0,j=0,color;
	LCD_SetWindow( sx, sy, ex-1, ey-1);
	LCD_WR_REG(0x2C);
	for(i=sx;i<ex;i++)
	{
		for(j=sy;j<ey;j++)
		{
			color=*arry;
			arry++;
			color = (color<<8) | (*arry);
            arry++;
			LCD_WR_16BITS_DATA(color);
		}
	}
}

	 

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void GUI_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		GUI_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
#include "math.h"
void GUI_DrawAngleLine(u16 x, u16 y, u16 r, int angle)
{
	u16 ex,ey;
	float a=angle*3.14/180;
	ex=x+cos(a)*r;
	ey=y-sin(a)*r;
	GUI_DrawLine(x,y,ex,ey);
}
void GUI_HLine(u16 x1,u16 y,u16 x2)
{
	u16 i=0;
	LCD_SetWindow(x1,y,x2-1,y);	
	LCD_WR_REG(0x2C);
	for(i=x1;i<x2;i++)
	{
		LCD_WR_16BITS_DATA(COLOR);	
	}
}
void GUI_VLine(u16 x,u16 y1,u16 y2)
{
	u16 i;
	for(i=y1;i<y2;i++)
	{
		GUI_DrawPoint(x,i);
	}
}


//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void GUI_DrawRect(u16 x1, u16 y1, u16 x2, u16 y2)
{
	GUI_HLine(x1,y1,x2);
	GUI_HLine(x1,y2-1,x2);
	GUI_VLine(x1,y1,y2);
	GUI_VLine(x2-1,y1,y2);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void GUI_DrawCircle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		GUI_DrawPoint(x0+a,y0-b);             //5
 		GUI_DrawPoint(x0+b,y0-a);             //0           
		GUI_DrawPoint(x0+b,y0+a);             //4               
		GUI_DrawPoint(x0+a,y0+b);             //6 
		GUI_DrawPoint(x0-a,y0+b);             //1       
 		GUI_DrawPoint(x0-b,y0+a);             
		GUI_DrawPoint(x0-a,y0-b);             //2             
        GUI_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=(a<<2)+6;	  
		else
		{
			di+=10+((a-b)<<2);   
			b--;
		} 						    
	}
} 									  
void  GUI_FillCircle(u32 x0, u32 y0, u32 r)
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
   if(draw_y0<LCD_HEIGHT)
   {  GUI_DrawPoint(draw_x0, draw_y0);	// 90度
   }
    	
   draw_x2 = draw_x3 = x0;
   draw_y2 = draw_y3 = y0 - r;
   if(draw_y2>=0)
   {  GUI_DrawPoint(draw_x2, draw_y2);	// 270度
   }
  	
   draw_x4 = draw_x6 = x0 + r;
   draw_y4 = draw_y6 = y0;
   if(draw_x4<LCD_WIDTH) 
   {  GUI_DrawPoint(draw_x4, draw_y4);	// 0度
      fill_x1 = draw_x4;
   }
   else
   {  fill_x1 = LCD_WIDTH;
   }
   fill_y0 = y0;							// 设置填充线条起始点fill_x0
   fill_x0 = x0 - r;						// 设置填充线条结束点fill_y1
   if(fill_x0<0) fill_x0 = 0;
   GUI_HLine(fill_x0, fill_y0, fill_x1);
   
   draw_x5 = draw_x7 = x0 - r;
   draw_y5 = draw_y7 = y0;
   if(draw_x5>=0) 
   {  GUI_DrawPoint(draw_x5, draw_y5);	// 180度
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
	  if( (draw_x0<=LCD_WIDTH)&&(draw_y0>=0) )	
	  {  GUI_DrawPoint(draw_x0, draw_y0);
	  }	    
	  if( (draw_x1>=0)&&(draw_y1>=0) )	
	  {  GUI_DrawPoint(draw_x1, draw_y1);
	  }
	  
	  /* 第二点水直线填充(下半圆的点) */
	  if(draw_x1>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x1;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y1;
         if(fill_y0>LCD_HEIGHT) fill_y0 = LCD_HEIGHT;
         if(fill_y0<0) fill_y0 = 0; 
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x1;				
         if(fill_x1>LCD_WIDTH) fill_x1 = LCD_WIDTH;
         GUI_HLine(fill_x0, fill_y0, fill_x1);
      }
	  
	  
	  if( (draw_x2<=LCD_WIDTH)&&(draw_y2<=LCD_HEIGHT) )	
	  {  GUI_DrawPoint(draw_x2, draw_y2);   
	  }
	    	  
	  if( (draw_x3>=0)&&(draw_y3<=LCD_HEIGHT) )	
	  {  GUI_DrawPoint(draw_x3, draw_y3);
	  }
	  
	  /* 第四点垂直线填充(上半圆的点) */
	  if(draw_x3>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x3;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y3;
         if(fill_y0>LCD_HEIGHT) fill_y0 = LCD_HEIGHT;
         if(fill_y0<0) fill_y0 = 0;
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x3;				
         if(fill_x1>LCD_WIDTH) fill_x1 = LCD_WIDTH;
         GUI_HLine(fill_x0, fill_y0, fill_x1);
      }
	  
	  	  
	  if( (draw_x4<=LCD_WIDTH)&&(draw_y4>=0) )	
	  {  GUI_DrawPoint(draw_x4, draw_y4);
	  }
	  if( (draw_x5>=0)&&(draw_y5>=0) )	
	  {  GUI_DrawPoint(draw_x5, draw_y5);
	  }
	  
	  /* 第六点垂直线填充(上半圆的点) */
	  if(draw_x5>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x5;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y5;
         if(fill_y0>LCD_HEIGHT) fill_y0 = LCD_HEIGHT;
         if(fill_y0<0) fill_y0 = 0;
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x5;				
         if(fill_x1>LCD_WIDTH) fill_x1 = LCD_WIDTH;
         GUI_HLine(fill_x0, fill_y0, fill_x1);
      }
	  
	  
	  if( (draw_x6<=LCD_WIDTH)&&(draw_y6<=LCD_HEIGHT) )	
	  {  GUI_DrawPoint(draw_x6, draw_y6);
	  }
	  
	  if( (draw_x7>=0)&&(draw_y7<=LCD_HEIGHT) )	
	  {  GUI_DrawPoint(draw_x7, draw_y7);
	  }
	  
	  /* 第八点垂直线填充(上半圆的点) */
	  if(draw_x7>=0)
	  {  /* 设置填充线条起始点fill_x0 */
	     fill_x0 = draw_x7;
	     /* 设置填充线条起始点fill_y0 */
	     fill_y0 = draw_y7;
         if(fill_y0>LCD_HEIGHT) fill_y0 = LCD_HEIGHT;
         if(fill_y0<0) fill_y0 = 0;
         /* 设置填充线条结束点fill_x1 */									
         fill_x1 = x0*2 - draw_x7;				
         if(fill_x1>LCD_WIDTH) fill_x1 = LCD_WIDTH;
         GUI_HLine(fill_x0, fill_y0, fill_x1);
      }
	  
   }
}




//0xA7A1-0xA7C1  0xA7D1-0xA7F1 66个俄文字符
//0xA140-0xA18B  76个亚美尼亚字符
void GUI_DispChar(s16 sx,s16 sy,const u16 ch,u8 mode)
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
	  
	W25Qxx_ReadBuffer(font,offset+BYTE_ADDR,36);
	for(x=0; x<BYTE_WIDTH; x++)
	{   
		temp=(font[i++]<<16)|(font[i++]<<8)|font[i++];
		
		for(y=0;y<BYTE_HEIGHT;y++)
		{			    
			if(temp&0x800000)GUI_DrawPixel(sx,sy+y,COLOR);
			else if(mode!=0)GUI_DrawPixel(sx,sy+y,BKCOLOR);
			temp<<=1;
		} 
		sx++;
	}  	
   	 	  
}

void GUI_DispHz(s16 sx,s16 sy,const u8 *hz,u8 mode)
{
	u8 		x,y,i=0;
	u8		font[72];
	u32 	offset=0;					//GBK Encode
	u32 	temp;
	
	
	if(hz[1]<0x7F) 
		offset=(190*(hz[0]-0x81)+hz[1]-0x40)*72;
	else 
		offset=(190*(hz[0]-0x81)+hz[1]-0x41)*72;
	
	W25Qxx_ReadBuffer(font,offset+WORD_ADDR,72);
	
	for(x=0;x<BYTE_WIDTH*2;x++)
	{   
		temp=(font[i++]<<16)|(font[i++]<<8)|font[i++];
		
		for(y=0;y<BYTE_HEIGHT;y++)
		{			    
			if(temp&0x800000)GUI_DrawPixel(sx,sy+y,COLOR);
			else if(mode!=0)GUI_DrawPixel(sx,sy+y,BKCOLOR);
			temp<<=1;
		} 
		sx++;
	}  	  
}

//返回值高4位：p的偏移量，*p<0x81 时，是单字节编码，p每次的偏移量为1. 
//                                    否则是双字节编码，p每次偏移量为2，
//      低4位：x坐标偏移量，ASCII字符、俄语字符、亚美尼亚字符 点阵宽度为 BYTE_WIDTH
//                          简体中文、日语 点阵宽度为 2个BYTE_WIDTH
u8 GUI_DispOne(s16 x, s16 y, const u8 *p, u8 mode)
{
    if(p==NULL || *p==0) return 0;
	
    if(*p<0x81)
    {			
        GUI_DispChar(x,y,*p,mode);
        return ((1<<4) | (1<<0));
    }
    else
    {
        if(isRussia(p) || isArmenian(p))
        {
            GUI_DispChar(x, y,(p[0]<<8)|p[1],mode);
            return ((2<<4) | (1<<0));
        }
        else
        {
            GUI_DispHz(x,y,p,mode);
            return ((2<<4) | (2<<0));
        }
    }
}    
const u8* GUI_DispString(s16 x, s16 y, const u8 *p, u8 mode)
{     
    u8 res = 0;    
	while(1)
	{   
        res= GUI_DispOne(x, y, p, mode);
        if(res == 0) return p;
        x += (res&0x0F) * BYTE_WIDTH; //res低四位，x坐标偏移
        p += (res&0xF0)>>4;           //res高四位，p偏移量
	}  
}

const u8* GUI_DispLenString(s16 x, s16 y, const u8 *p, u8 mode, u8 len)
{       
    u8 res = 0, nlen = 0;
	while(nlen < len)
	{   
        res= GUI_DispOne(x, y, p, mode);
        if(res == 0) return p;
        x += (res&0x0F) * BYTE_WIDTH; //res低四位，x坐标偏移
        nlen += (res&0x0F);
        p += (res&0xF0)>>4;           //res高四位，p偏移量
	}
    return p;
}

void GUI_DispStringRight(s16 x, s16 y, const u8 *p,u8 mode)
{
	x -= my_strlen(p) * BYTE_WIDTH; 
    GUI_DispString(x, y, p, mode);
}

void GUI_DispStringInRect(s16 sx,s16 sy,s16 ex, s16 ey, const u8 *p, u8 mode)
{    
    u16 stringlen = my_strlen(p)*BYTE_WIDTH;
    u16 width = ex - sx;
    u16 height = ey - sy;
    u8  nline = (stringlen+width-1)/width ;
    
    if(nline > height/BYTE_HEIGHT)
        nline = height/BYTE_HEIGHT;

    u16 x_offset = stringlen >= width ? 0 : ( width-stringlen)>>1;
    u16 y_offset = (nline*BYTE_HEIGHT) >= height ? 0 : (( height - (nline*BYTE_HEIGHT) )>>1);
    u16 x = sx + x_offset, y = sy + y_offset;
    
    u8 i=0;
    for(i=0; i<nline; i++)
    {
        p = GUI_DispLenString(x,y,p,mode,width/BYTE_WIDTH);
        y += BYTE_HEIGHT;
    }   
}

void GUI_DispStringInPrect(const GUI_RECT *rect, const u8 *p,u8 mode)
{    
    GUI_DispStringInRect(rect->x0, rect->y0, rect->x1, rect->y1,p,mode);
}
void GUI_DispCharInPrect(const GUI_RECT *rect, u16 p,u8 mode)
{    
	u16 x_offset = (rect->x1 - rect->x0 - BYTE_WIDTH)>>1;
	u16 y_offset = (rect->y1 - rect->y0 - BYTE_HEIGHT)>>1;
	GUI_DispChar(rect->x0+x_offset,rect->y0+y_offset,p,mode);	
}


const u32 GUI_Pow10[10] = {
  1 , 10, 100, 1000, 10000,
  100000, 1000000, 10000000, 100000000, 1000000000
};

void GUI_DispDec(s16 x,s16 y,s32 num,u8 len,u8 mode,u8 leftOrRight)
{         	
	u8 t,temp,i=0;
	u8 enshow=0;	
    bool isNegative = false;
    if(num<0)
    {
        num = -num;
        isNegative = true;
    }        
	for(t=0;t<len;t++)
	{
		temp=(num/GUI_Pow10[len-t-1])%10;
		if(enshow==0)
		{
			if(temp==0 && t<(len-1))
			{
				if(leftOrRight==RIGHT)
				{
					GUI_DispChar(x,y,' ',mode);
					x += BYTE_WIDTH;
				}
				else
				{
					i++;
				}
				continue;
			}
            else 
            {
                enshow=1; 
                if(isNegative == true)
                {                
                    GUI_DispChar(x-BYTE_WIDTH,y,'-',mode);
                }
            }
		 	 
		}
	 	GUI_DispChar(x,y,temp+'0',mode); 
		x += BYTE_WIDTH;
	}
	for(;i>0;i--)
	{
	 	GUI_DispChar(x,y,' ',mode); 
		x += BYTE_WIDTH;
	}
} 

void GUI_DispFloat(s16 x,s16 y,float num,u8 llen,u8 rlen)
{    
    u8  alen = 0;
	u8  i=0;
    u8  notZero = 0;
	GUI_DispChar(x,y,num>=0 ? ' ':'-',1);
	x += BYTE_WIDTH;
	num= num>=0 ? num : (-num);
	for(i=0;i<llen;i++)
	{
        u8 bit_value = ((u32)(num/GUI_Pow10[llen-1-i]))%10;
        if(bit_value !=0 || notZero !=0 || i == llen-1)
        {
            notZero = 1;
            GUI_DispChar(x,y,bit_value+'0',1);
            x += BYTE_WIDTH;
            alen++;
        }
	}
	GUI_DispChar(x,y,'.',1);
	x += BYTE_WIDTH;
    alen++;

    num *= GUI_Pow10[(unsigned)rlen];
    num += 0.5;
    num = (float) floor (num);
	for(i=0;i<rlen;i++)
	{
		GUI_DispChar(x,y,(int)(num/GUI_Pow10[rlen-1-i])%10+'0',1);
        x += BYTE_WIDTH;
        alen++;
	}
    for(; alen < llen+rlen; alen++)
    {        
		GUI_DispChar(x,y,' ',1);
        x += BYTE_WIDTH;
    }
    
} 




/****************************************************     Widget    *******************************************************************/
void RADIO_Create(RADIO *raido)
{
	u8 i=0;
	for(i=0;i<raido->num;i++)
	{
		if(i==raido->select)
			GUI_SetColor(0x661F);
		else
			GUI_SetColor(WHITE);
		GUI_FillCircle(raido->sx+5,i*raido->distance+raido->sy+8,3);
		GUI_DrawCircle(raido->sx+5,i*raido->distance+raido->sy+8,5);
		GUI_DispString(raido->sx+15,i*raido->distance+raido->sy,raido->context[i],1);
	}	
	GUI_SetColor(WHITE);
}

void RADIO_Select(RADIO *raido, u8 select)
{
	u8 i=0;
	if(raido->select==select)
		return;
	for(i=0;i<2;i++)
	{
		if(i==0)
		{
			GUI_SetColor(WHITE);
		}
		else
		{
			raido->select=select;
			GUI_SetColor(0x661F);
		}
		GUI_FillCircle(raido->sx+5,raido->select*raido->distance+raido->sy+8,3);
        GUI_DrawCircle(raido->sx+5,raido->select*raido->distance+raido->sy+8,5);
		GUI_DispString(raido->sx+15,raido->select*raido->distance+raido->sy,raido->context[raido->select],1);
	}
	GUI_SetColor(WHITE);
}

//滚动显示字符串
void Scroll_CreatePara(SCROLL * para, u8 *pstr ,GUI_RECT *rect)
{
	memset(para,0,sizeof(SCROLL));	
	para->text = pstr;
	para->len_size = strlen((char *)pstr);    
	para->len_total = my_strlen(pstr);
	para->len_max = ((rect->x1 - rect->x0)/BYTE_WIDTH);
	para->rect = rect;
}

int min(int value1,int value2)
{
  if(value1>value2)
    return value2;
  return value1;  
}

int max(int value1,int value2)
{
  if(value1>value2)
    return value1;
  return value2;  
}

void Scroll_DispString(SCROLL * para,u8 mode,u8 align)
{
    u16 i=0;		

    if(para->text==NULL) return;
    if(para->len_total > para->len_max)
    {
        if(OS_GetTime()-para->time>5)
        {
            para->time=OS_GetTime();			      
            GUI_SetRange(para->rect->x0, para->rect->y0, para->rect->x1, para->rect->y1);
            if(para->index < para->len_size)
            {
                para->off_head++;
                if((u8)para->text[para->index]<0x81)
                {
                    if(para->off_head==BYTE_WIDTH) {para->index++; para->off_head=0;}
                }
                else if( isRussia(&para->text[para->index]) || isArmenian(&para->text[para->index]) )
                {
                    if(para->off_head==BYTE_WIDTH) {para->index += 2; para->off_head=0;}      
                }
                else
                {
                    if(para->off_head==BYTE_WIDTH*2) {para->index += 2; para->off_head=0;}      
                }

                GUI_DispLenString(para->rect->x0 - para->off_head, para->rect->y0 ,&para->text[para->index],mode,para->len_max+4);

                for(i=para->rect->y0;i<para->rect->y1;i++)					//清除拖尾
                {
                    GUI_DrawPixel(para->rect->x0 - para->off_head + (min(para->len_size - para->index, para->len_max + 4) * BYTE_WIDTH), i, BKCOLOR);
                }
            }      
            if(para->len_size - para->index < para->len_max-4)  //首尾相差四个字符的间隔
            {
                para->off_tail++;
                GUI_DispLenString(para->rect->x1-para->off_tail,para->rect->y0,para->text,mode,(para->off_tail + BYTE_WIDTH - 1)/BYTE_WIDTH);
                if(para->off_tail >= para->rect->x1-para->rect->x0)
                {
                    para->off_head=0;
                    para->off_tail=0;
                    para->index=0;
                }
            }		
            GUI_CancelRange();
        }
    }
    else if(para->has_disp==false)
    {
        switch(align)
        {
            case LEFT: 
            {
                GUI_DispString(para->rect->x0,para->rect->y0,para->text,mode);
                break;
            }
            case RIGHT: 
            {
                u16 x_offset=(para->rect->x1 - (para->len_total * BYTE_WIDTH));	
                GUI_DispString(x_offset,para->rect->y0,para->text,mode);
                break;
            }
            case CENTER:
            {
                u16 x_offset=((para->rect->x1 - para->rect->x0 - (para->len_total * BYTE_WIDTH)) >>1);	
                GUI_DispString(para->rect->x0+x_offset,para->rect->y0,para->text,mode);
                break;
            }
        }
        para->has_disp=true;
    }
}


//绘制按键
void GUI_DrawButton(const BUTTON *button, bool pressed)
{
    const u16 radius = button->radius;
    const u16 lineWidth = button->lineWidth;
    const s16 sx = button->rect.x0,
              sy = button->rect.y0,
              ex = button->rect.x1,
              ey = button->rect.y1;
    const u16 nowBackColor = GUI_GetBkColor();
    const u16 nowFontColor = GUI_GetColor();
    
    const u16 lineColor = pressed ? button->pLineColor : button->lineColor;
    const u16 backColor = pressed ? button->pBackColor : button->backColor;
    const u16 fontColor = pressed ? button->pFontColor : button->fontColor;
    
    GUI_SetColor(lineColor);
    GUI_FillCircle(sx + radius,      sy + radius,  radius);   //四个角的圆弧
    GUI_FillCircle(ex - radius - 1,  sy + radius,  radius);
    GUI_FillCircle(sx + radius,     ey - radius - 1, radius);
    GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius);
    
    for(u16 i=0; i<lineWidth ;i++)
    {
        GUI_HLine(sx + radius, sy + i,      ex - radius);  //四个外边
        GUI_HLine(sx + radius, ey - 1 - i,  ex - radius);
        GUI_VLine(sx + i,      sy + radius, ey - radius);
        GUI_VLine(ex - 1 - i,  sy + radius, ey - radius);
    }
    GUI_SetColor(backColor);
    GUI_FillCircle(sx + radius,      sy + radius,  radius - lineWidth);   //清除四个角的圆弧
    GUI_FillCircle(ex - radius - 1,  sy + radius,  radius - lineWidth);
    GUI_FillCircle(sx + radius,     ey - radius - 1, radius - lineWidth);
    GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius - lineWidth);
    GUI_FillRect(sx + radius, sy + lineWidth, ex - radius, sy + lineWidth + radius);
    GUI_FillRect(sx + lineWidth, sy + lineWidth + radius, ex - lineWidth, ey - lineWidth - radius);
    GUI_FillRect(sx + radius, ey - lineWidth - radius, ex - radius, ey - lineWidth);
    
    GUI_SetColor(fontColor);
    GUI_DispStringInPrect(&button->rect, button->context, 0);
   
    GUI_SetBkColor(nowBackColor);
    GUI_SetColor(nowFontColor);
}


void GUI_DrawWindow(const WINDOW *window, const u8 *title, const u8 *inf)
{
    const u16 titleHeight = window->title.height;
    const u16 infoHeight = window->info.height;
    const u16 radius = window->radius;
    const u16 lineWidth = window->lineWidth;
    const u16 lineColor = window->lineColor;
    const u16 infoBackColor = window->info.backColor;
    const u16 bottomBackColor = window->bottom.backColor;
    const s16 sx = window->rect.x0,
              sy = window->rect.y0,
              ex = window->rect.x1,
              ey = window->rect.y1;
    const u16 nowBackColor = GUI_GetBkColor();
    const u16 nowFontColor = GUI_GetColor();
    
    GUI_SetColor(lineColor);
    GUI_FillCircle(sx + radius,      sy + radius,  radius);
    GUI_FillCircle(ex - radius - 1,  sy + radius,  radius);
    GUI_FillRect(sx + radius, sy,        ex-radius, sy+radius);
    GUI_FillRect(sx,          sy+radius, ex,        sy+titleHeight);
    for(u16 i=0; i<lineWidth ;i++)
    {
        GUI_VLine(sx + i,      sy + titleHeight, ey - radius);
        GUI_VLine(ex - 1 - i,  sy + titleHeight, ey - radius);
        GUI_HLine(sx + radius, ey - 1 - i,       ex - radius);
    }
    GUI_FillCircle(sx + radius,     ey - radius - 1, radius);
    GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius);
    
    GUI_SetColor(infoBackColor);
    GUI_FillRect(sx + lineWidth, sy + titleHeight, ex - lineWidth, sy + titleHeight + infoHeight);
    GUI_SetColor(bottomBackColor);
    GUI_FillCircle(sx + radius,     ey - radius - 1, radius - lineWidth);
    GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius - lineWidth);
    GUI_FillRect(sx + lineWidth,          sy + titleHeight + infoHeight, ex - lineWidth,          ey - lineWidth - radius);
    GUI_FillRect(sx + lineWidth + radius, ey - lineWidth - radius,       ex - lineWidth - radius, ey - lineWidth);
    
    GUI_SetColor(window->title.fontColor);
//    GUI_DispStringInRect(rect.x0, rect.y0, rect.x1, rect.y0+titleHeight,title,0);
    GUI_DispString(sx+radius, sy+8, title,0);
    GUI_SetColor(window->info.fontColor);
    GUI_DispStringInRect(sx+lineWidth+BYTE_WIDTH, sy+titleHeight, ex-lineWidth-BYTE_WIDTH, sy+titleHeight+infoHeight, inf, 0); 
    
    GUI_SetBkColor(nowBackColor);
    GUI_SetColor(nowFontColor);
}
