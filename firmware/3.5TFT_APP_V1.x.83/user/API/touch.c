#include "includes.h"
#include "stm32f10x_flash.h"
#include "touch.h"
#include "stdio.h"
/*
函数：
GPIO_Init();
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

LCD?        STM32

DOUT   MISO   PF8  输入
DIN    MOSI   PF9  输出
PEN    T_PEN  PF10 输入
CS     T_CS   PB2  输出
DCLK   CLK    PB1  输出
*/

/*******************************************************************************
功能描述：读取STM32flash指定地址的内容
输入参数；u32 faddr：地址
输出参数；地址对应的u32内容
备    注: 无
*************************************************************************/
u32 STMFLASH_ReadHalfWord(u32 faddr)
{
	return*(vu32*)faddr;
}

/*******************************************************************************
功能描述：读取保存的 A B C D E F K的值
输入参数；u32 faddr：地址
输出参数；void
备    注: 无
*************************************************************************/
void STMFLASH_Read_Calibration(void)
{
	A = STMFLASH_ReadHalfWord(ADJUST_IDADDR+4);
	B = STMFLASH_ReadHalfWord(ADJUST_IDADDR+8);
	C = STMFLASH_ReadHalfWord(ADJUST_IDADDR+12);
	D = STMFLASH_ReadHalfWord(ADJUST_IDADDR+16);
	E = STMFLASH_ReadHalfWord(ADJUST_IDADDR+20);
	F = STMFLASH_ReadHalfWord(ADJUST_IDADDR+24);
	K = STMFLASH_ReadHalfWord(ADJUST_IDADDR+28);
	
//	printf("\n\rA=%d\n\r",A);
//	printf("\n\rB=%d\n\r",B);
//	printf("\n\rC=%d\n\r",C);
//	printf("\n\rD=%d\n\r",D);
//	printf("\n\rE=%d\n\r",E);
//	printf("\n\rF=%d\n\r",F);
//	printf("\n\rK=%d\n\r",K);
}
/*******************************************************************************
功能描述：存储三点校准值到FLASH
输入参数；u32 faddr：地址
输出参数；void
备    注: 无
*************************************************************************/
void STMFLASH_Store_Cost(u32 faddr)
{
		FLASH_Unlock();///解锁FLASH
		FLASH_EraseOptionBytes();
		FLASH_ErasePage(faddr);
		FLASH_ProgramWord(faddr, ADJUST_COMPLETE);
	
		FLASH_ProgramWord(faddr+4,  A);
		FLASH_ProgramWord(faddr+8,  B);
		FLASH_ProgramWord(faddr+12, C);
		FLASH_ProgramWord(faddr+16, D);
		FLASH_ProgramWord(faddr+20, E);
		FLASH_ProgramWord(faddr+24, F);
		FLASH_ProgramWord(faddr+28, K);
}


/****************************************************
函数名：Read_Lcd_xy
函数功能：检查E2PROM中是否存有三点校准的坐标值。若有，读取出来。若无，进行三点校准，将校准值存入E2PROM
形参：无
返回值：无
****************************************************/	
void Read_Lcd_xy(void)
{
	u32 TEMP_ID;
	TEMP_ID=STMFLASH_ReadHalfWord(ADJUST_IDADDR);
	if(TEMP_ID==ADJUST_COMPLETE)
	{
		STMFLASH_Read_Calibration();
	}
	else
	{
		Three_adjust();
		STMFLASH_Store_Cost(ADJUST_IDADDR);
	}
}


/****************************************************
函数名：TP_GPIOConfig
形参：
返回值：
函数功能：触摸屏初始化
****************************************************/
void TP_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//输出 PC0 -- T_CS -- CS  PC1 -- CLK -- DCLK  PC3 -- MOSI -- DOUT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_3;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	
	//输入 PC2 -- MISO -- DIN PC4 -- T_PEN -- PEN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);	
}




#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
/****************************************************
函数名：TP_Read_AD
形参：
返回值：
函数功能：读取触摸屏转化好的AD值
****************************************************/
u16 TP_Read_AD(u8 CMD)
{
	u16 ADNum;
	TCS(0);
	TCLK(0);
	TOUT(0);
	
	TP_Write_Cmd(CMD);
	Delay_us(10);
	
	TCLK(0);
	Delay_us(1);
	TCLK(1);
	
	TP_Read_Data(&ADNum);
	ADNum >>= 4;
	
	TCS(1);
	return ADNum;
}

/****************************************************
函数名：TP_Write_Cmd
形参：  CMD要发送的命令
返回值：
函数功能：给触摸屏控制器发送命令
****************************************************/
void TP_Write_Cmd(u8 CMD)
{
	u16 counter = 0;
	u8 CMD_Mask = 0x80;
	TCLK(0);
	for(counter=0;counter<8;counter++)
	{
		if(CMD & CMD_Mask) TOUT(1);
		else TOUT(0);
		TCLK(1);
		TCLK(0);
		CMD <<= 1;
	}
	
}
/****************************************************
函数名：TP_Read_Data
形参：  *AD_Data
返回值：
函数功能：读取触摸屏值
****************************************************/
void TP_Read_Data(u16 *AD_Data)
{
	u16 counter;
	for(counter = 0;counter<16;counter++)
	{
		TCLK(0);
		TCLK(1);
		(*AD_Data)<<=1;
		if(TDIN()) (*AD_Data)++;
	}
}
/****************************************************
函数名：TP_Read_xy
形参：  *x -- x轴坐标
        *y -- y轴坐标
返回值：
函数功能：读取触摸屏x y的坐标值
****************************************************/
void TP_Read_xy(u16 *x,u16 *y)
{
	u16 temp_x,temp_y;
	temp_x = TP_Read_XOY(CMD_RDX);
	temp_y = TP_Read_XOY(CMD_RDY);
	
	*x = temp_x;
	*y = temp_y;
}
/****************************************************
函数名：Test_touch
形参：
返回值：
函数功能：测试触摸屏
****************************************************/
//void Test_touch(void)
//{
//		char buffer_x[10] = {"x=       "};
//		char buffer_y[10] = {"y=       "};
//		u16 tp_x,tp_y;
//		while(TPEN());
//		TP_Read_xy(&tp_x,&tp_y);
//		printf("\n\r触摸屏x轴：%d\n\r",tp_x);
//		printf("\n\r触摸屏y轴：%d\n\r",tp_y);
//		my_itoa(tp_x,buffer_x+2,10);
//		my_itoa(tp_y,buffer_y+2,10);
//		Draw_Text_8_16_Str(20,120,RED,WHITE,(u8 *)buffer_x);
//		Draw_Text_8_16_Str(20,160,RED,WHITE,(u8 *)buffer_y);
//		while(!TPEN());
//}
//用于校准算法的转化
#define XL1 LCD_X[0]
#define XL2 LCD_X[1]
#define XL3 LCD_X[2]

#define YL1 LCD_Y[0]
#define YL2 LCD_Y[1]
#define YL3 LCD_Y[2]

#define X1  TP_X[0]
#define X2  TP_X[1]
#define X3  TP_X[2]

#define Y1  TP_Y[0]
#define Y2  TP_Y[1]
#define Y3  TP_Y[2]
/****************************************************
函数名：Three_adjust()
形参：
返回值：
函数功能：三点校准
****************************************************/
u32 A,B,C,D,E,F,K;//校准系数
void Three_adjust(void)
{

	u32 LCD_X[3] = {80,400,400};
	u32 LCD_Y[3] = {80,80,250};
	u16 TP_X[3],TP_Y[3];
	u32 tp_num = 0;
	int i;
	LCD_Clear(WHITE);
	GUI_SetColor(BLACK);
	GUI_SetBKColor(WHITE);
	GUI_DispString(100,10,"Touch Screen Calibration",0);
	GUI_DispString(100,34,"Please click on the red dot",0);
	for(tp_num = 0;tp_num<3;tp_num++)
	{
			GUI_CircleFill(LCD_X[tp_num],LCD_Y[tp_num],3,RED);
		for(i=0;i<10;i++)
		{
			LCD_DrawPoint(LCD_X[tp_num]+i,LCD_Y[tp_num],RED);
			LCD_DrawPoint(LCD_X[tp_num]-i,LCD_Y[tp_num],RED);
			LCD_DrawPoint(LCD_X[tp_num],LCD_Y[tp_num]+i,RED);
			LCD_DrawPoint(LCD_X[tp_num],LCD_Y[tp_num]-i,RED);
		}
		while(!press);
		TP_Read_xy(&TP_X[tp_num],&TP_Y[tp_num]);
		Delay_ms(200);
		while(press);
	}
	
	K = (X1 - X3)*(Y2 - Y3) - (X2 - X3)*(Y1 - Y3);
	A = ((XL1 - XL3)*(Y2 - Y3) - (XL2 - XL3)*(Y1 - Y3));
	B = (( X1 - X3 )*( XL2 - XL3) - (XL1 - XL3)*( X2 - X3));
	C = (Y1*( X3*XL2 - X2*XL3) + Y2*(X1*XL3 - X3*XL1) + Y3*(X2*XL1 - X1*XL2));
	D = ((YL1 - YL3)*(Y2 - Y3) - (YL2 - YL3)*(Y1 - Y3));
	E = ((X1 - X3)*(YL2 - YL3) - (YL1 - YL3)*(X2 - X3));
	F = (Y1*(X3*YL2 - X2*YL3) + Y2*(X1*YL3 - X3*YL1) + Y3*(X2*YL1 - X1*YL2));
	
//	printf("\n\rA=%d\n\r",A);
//	printf("\n\rB=%d\n\r",B);
//	printf("\n\rC=%d\n\r",C);
//	printf("\n\rD=%d\n\r",D);
//	printf("\n\rE=%d\n\r",E);
//	printf("\n\rF=%d\n\r",F);
//	printf("\n\rK=%d\n\r",K);
	Adjust_test(200,180);
	

}
/****************************************************
函数名：Adjust_test
形参：  x -- 检测的x坐标
        y -- 检测的y坐标
返回值：
函数功能：检测校准是否正确
****************************************************/
void Adjust_test(u16 x,u16 y)
{
	u32 i;
	u8 buf[20];
	u16 tp_x,tp_y;
	int lcd_x,lcd_y;
	GUI_CircleFill(x,y,5,BLACK);
	GUI_SetColor(RED);
	GUI_DispString(100,100,"Cur Position (200,180)",0);
	for(i=0;i<10;i++)
	{
		LCD_DrawPoint(x+i,y,BLACK);
		LCD_DrawPoint(x-i,y,BLACK);
		LCD_DrawPoint(x,y+i,BLACK);
		LCD_DrawPoint(x,y-i,BLACK);
	}
	while(!press);
	TP_Read_xy(&tp_x,&tp_y);
//	printf("\n\rtp_x = %d\n\r",tp_x);
//	printf("\n\rtp_y = %d\n\r",tp_y);
//	
	lcd_x = (A*tp_x+B*tp_y+C)/K;
	lcd_y = (D*tp_x+E*tp_y+F)/K;
	
//	printf("\n\rlcd_x = %d \n\rlcd_y = %d\n\r\n\r",lcd_x,lcd_y);
	sprintf((void *)buf,"Test Position (%d,%d)",lcd_x,lcd_y);
	GUI_DispString(100,130,buf,0);
	if(lcd_x < x+20 && lcd_x>x-20  && lcd_y > y-20 && lcd_y<y+20)
	{
		GUI_SetColor(BLACK);
		GUI_DispString(80,240,"Adjust OK",0);
		GUI_SetColor(WHITE);
		GUI_SetBKColor(BLACK);
		Delay_ms(1000);
		LCD_Clear(BLACK);
	}
	else
	{
		while(press);
		GUI_DispString(20,240,"Adjust failed,Please again",0);
		Delay_ms(1000);
		Three_adjust();
	}
}

#define ERR_RANGE 50 //误差范围 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
    TP_Read_xy(&x1,&y1);   
    TP_Read_xy(&x2,&y2);	 
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 

void TP_Read_Place(u16 *x, u16 *y)
{
	u16 tp_x=0,tp_y=0;
	if(TP_Read_XY2(&tp_x,&tp_y))
	{		
		*x = (A*tp_x+B*tp_y+C)/K;
		*y = (D*tp_x+E*tp_y+F)/K;
	}
	else
	{
		*x=0;
		*y=0;
	}
}
u8 Get_Place(u8 view_flag)
{
	static u8 t_press=0;
	static u8 key_num=0;
	u8 key_return=0;
	u16 x=0,y=0;
	
	if(press)
	{
		if(t_press==0)
		{
			//获得LCD屏坐标值
			TP_Read_Place(&x,&y);
			key_num=Key_value(x,y);
				if(key_num==0)
					return 0;
			Back_display(1,view_flag,key_num);
			t_press=1; 
	
		}
	}	
	else
	{
		if(t_press==1)
		{
			Back_display(0,view_flag,key_num);
			key_return=key_num;
			t_press=0;
			key_num=0;
		}
	}

	return key_return;
}


u8 Key_value(u16 x,u16 y)
{
	u8 i;

	switch(cur_view)
	{
		case MENU_PRINT:
			for(i=0;i<ACOUNT(menuRectPrint);i++)
			{
			
				if(x>menuRectPrint[i].x0 && x<menuRectPrint[i].x1 && y>menuRectPrint[i].y0 && y<menuRectPrint[i].y1)
				{
						return i+1;
				}
			}
			break;
		case MENU_OUTAGE:
			for(i=0;i<ACOUNT(menuRectOutage);i++)
			{
				if(x>menuRectOutage[i].x0 && x<menuRectOutage[i].x1 && y>menuRectOutage[i].y0 && y<menuRectOutage[i].y1)
				{
						return i+1;
				}
			}
			break;
		default:
			for(i=0;i<ACOUNT(menuRect);i++)
			{
				if(x>menuRect[i].x0 && x<menuRect[i].x1 && y>menuRect[i].y0 && y<menuRect[i].y1)
				{
						return i+1;
				}
			}
			break;
	}
	return 0;
}



void Back_display(u8 press,u8 view_flag,u8 key_num)
{
	if(key_num==8&&view_flag!=MENU_MAIN&&view_flag!=MENU_PRINTING&&view_flag!=MENU_OUTAGE)
	{
		if(press)
			Read_Display_Photo(7,BACK_ICON,1);
	}
	else
	{
		switch(view_flag)
		{
			case MENU_MAIN:
				if(key_num!=8)
				{
					if(press)
					{
						Read_Display_Photo(key_num-1,key_num-1,1);
					}			
				}
				else
				{
					Read_Display_Photo(7,LEVELING_ICON,press);
				}
				break;
			case MENU_HEAT:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,ADD_ICON,press);break;
					case 4:Read_Display_Photo(3,SUB_ICON,press);break;
					case 5:
						if(press)
						{
						if(infoHeat.type==NOZZLE)
							Read_Display_Photo(4,NOZZLE_ICON,1);
						else
							Read_Display_Photo(4,BED_ICON,1);
						}
						break;
					case 6:
						if(press)
						{
						if(infoHeat.heat_unit==1)
							Read_Display_Photo(5,DEGREE_ICON_1,1);
						else if(infoHeat.heat_unit==5)
							Read_Display_Photo(5,DEGREE_ICON_5,1);
						else
							Read_Display_Photo(5,DEGREE_ICON_10,1);
						}
						break;
					case 7:Read_Display_Photo(6,STOP_ICON,press);break;
					default:break;
				}				
				break;
			case MENU_MOVE:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,XADD_ICON,press);break;
					case 2:Read_Display_Photo(1,YADD_ICON,press);break;
					case 3:Read_Display_Photo(2,ZADD_ICON,press);break;
					case 4:
						if(press)
						{
							if(infoMove.move_unit==0)
								Read_Display_Photo(3,MMM_01_ICON,1);
							else if(infoMove.move_unit==1)
								Read_Display_Photo(3,MMM_1_ICON,1);
							else
								Read_Display_Photo(3,MMM_10_ICON,1);
						}
						break;
					case 5:Read_Display_Photo(4,XSUB_ICON,press);break;
					case 6:Read_Display_Photo(5,YSUB_ICON,press);break;
					case 7:Read_Display_Photo(6,ZSUB_ICON,press);break;
					default:break;
				}				
				break;;
			case MENU_HOME:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,HOME_ICON,press);break;
					case 2:Read_Display_Photo(1,ZERO_X_ICON,press);break;
					case 3:Read_Display_Photo(2,ZERO_Y_ICON,press);break;
					case 4:Read_Display_Photo(3,ZERO_Z_ICON,press);break;
					default:break;
				}				
				break;
			case MENU_PRINT:
				if(key_num<=5)
				{
					if(infoPrint.back_icon[key_num-1]!=0)
					{
						Read_Display_Photo(key_num-1,infoPrint.back_icon[key_num-1],press);
					}
				}
				else
				{
					switch(key_num)
					{
						case 6:Read_Display_Photo(5,LAST_ICON,press);break;
						case 7:Read_Display_Photo(6,NEXT_ICON,press);break;
						default:break;
					}		
				}				
				break;
			case MENU_PRINTING:
				if(press)
				{
					switch(key_num)
					{
						case 1:
							if(infoPrinting.pause==0)
								Read_Display_Photo(0,PAUSE_ICON,1);
							else
								Read_Display_Photo(0,RESUME_ICON,1);
							break;
						case 4:
								Read_Display_Photo(3,STOP_ICON,1);
							break;
						case 5:
								Read_Display_Photo(4,HEAT_ICON,1);	
							break;					
						case 6:
								Read_Display_Photo(5,FAN_ICON,1);
							break;
						case 7:
								Read_Display_Photo(6,SPEED_ICON,1);
							break;
						case 8:
								Read_Display_Photo(7,RESERVE_1_ICON,1);
							break;
						default:break;
					}				
				}
				break;
			case MENU_SPEED:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,ADD_ICON,press);break;
					case 4:Read_Display_Photo(3,SUB_ICON,press);break;
					case 7:
						if(press)
						{
							if(infoPrinting.speed_unit==1)
								Read_Display_Photo(6,EMM_ICON_1,1);
							else if(infoPrinting.speed_unit==5)
								Read_Display_Photo(6,EMM_ICON_5,1);
							else
								Read_Display_Photo(6,EMM_ICON_10,1);							
						}
						break;
					default:break;
				}				
				break;	
			case MENU_BABY:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,ADD_ICON,press);break;
					case 4:Read_Display_Photo(3,SUB_ICON,press);break;
					default:break;
				}				
				break;
			case MENU_EXTRUDER:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,EADD_ICON,press);break;
					case 4:Read_Display_Photo(3,ESUB_ICON,press);break;
					case 5:Read_Display_Photo(4,NOZZLE_ICON,press);break;
					case 6:
						if(press)
						{
							if(infoExtrude.speed_unit==60)
								Read_Display_Photo(5,SLOW_ICON,1);
							else if(infoExtrude.speed_unit==600)
								Read_Display_Photo(5,NORMAL_ICON,1);
							else
								Read_Display_Photo(5,FAST_ICON,1);							
						}
						break;
					case 7:
						if(press)
						{
							if(infoExtrude.move_unit==1)
								Read_Display_Photo(6,EMM_ICON_1,1);
							else if(infoExtrude.move_unit==5)
								Read_Display_Photo(6,EMM_ICON_5,1);
							else
								Read_Display_Photo(6,EMM_ICON_10,1);							
						}
						break;
					default:break;
				}				
				break;
			case MENU_FAN:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,ADD_ICON,press);break;
					case 4:Read_Display_Photo(3,SUB_ICON,press);break;
					case 5:Read_Display_Photo(4,FULL_ICON,press);break;
					case 6:Read_Display_Photo(5,HALF_ICON,press);break;
					case 7:Read_Display_Photo(6,STOP_ICON,press);break;
					default:break;
				}				
				break;
			case MENU_SETTING:
				switch(key_num)
				{
					case 1:Read_Display_Photo(0,CLOSE_ICON,press);break;
					case 2:
						if(press)
						{
							Read_Display_Photo(1,LANGUE_ICON,press);
						}
						break;
					case 3:
						if(press)
						{
							Read_Display_Photo(2,ADJUST_ICON,1);
						}
						break;
					case 4:
						if(press)
						{
							Read_Display_Photo(3,ABOUT_ICON,press);
						}
						break;
					case 5:
						if(press)
						{
							Read_Display_Photo(4,DISCONNECT_ICON,1);
						}
						break;
					case 6:Read_Display_Photo(5,RESET_ICON,press); break;

				}				
				break;
			case MENU_OUTAGE:
				if(press)
				{
					switch(key_num)
					{
						u8 x_offset;
						case 1:						
							GUI_FillRect(menuRectOutage[0].x0,menuRectOutage[0].y0,menuRectOutage[0].x1,menuRectOutage[0].y1,BLUE);
							x_offset=(menuRectOutage[0].x1-menuRectOutage[0].x0-(my_strlen(textSelect(CONTINUE))*12))>>1;
							GUI_DispString(menuRectOutage[0].x0+x_offset ,menuRectOutage[0].y0+((menuRectOutage[0].y1-menuRectOutage[0].y0-24)>>1),textSelect(CONTINUE),0);	
							break;
						case 2:
							GUI_FillRect(menuRectOutage[1].x0,menuRectOutage[1].y0,menuRectOutage[1].x1,menuRectOutage[1].y1,BLUE);
							x_offset=(menuRectOutage[1].x1-menuRectOutage[1].x0-(my_strlen(textSelect(CANCEL))*12))>>1;
							GUI_DispString(menuRectOutage[1].x0+x_offset,menuRectOutage[0].y0+((menuRectOutage[1].y1-menuRectOutage[1].y0-24)>>1),textSelect(CANCEL),0);
							break;
					}			
				}				
				break;
			default:break;
		}
	}
}



