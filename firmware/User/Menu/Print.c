#include "Print.h"
#include "myfatfs.h"
#include "includes.h"

//1个title(标题), ITEM_PER_PAGE个item(图标+标签) 
MENUITEMS printItems = {
//  title
    LABEL_BACKGROUND,
//   icon                       label
   {{ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_PAGE_UP,              LABEL_PAGE_UP},
    {ICON_PAGE_DOWN,            LABEL_PAGE_DOWN},
    {ICON_BACK,                 LABEL_BACK},}
};

/* 打印文件列表界面 */
#define NUM_PER_PAGE	5

SCROLL   titleScroll;
GUI_RECT titleRect={10,10,470,10+BYTE_HEIGHT};

SCROLL   gcodeScroll;
GUI_RECT gcodeRect[NUM_PER_PAGE]=
{ 
    {5,     160,    115,    160+BYTE_HEIGHT},
    {125,	160,    235,    160+BYTE_HEIGHT},
    {245,	160,    355,    160+BYTE_HEIGHT},
    {365,   160,    475,    160+BYTE_HEIGHT},

    {5,     290,    115,    290+BYTE_HEIGHT},
};


void scrollFileNameCreate(u8 i)
{
	u8 num=infoFile.cur_page * NUM_PER_PAGE + i;	
	
	if(infoFile.F_num + infoFile.f_num==0)
	{
		memset(&gcodeScroll,0,sizeof(SCROLL));
		return;
	}
	if(num<infoFile.F_num)
	{
        Scroll_CreatePara(&gcodeScroll, (u8* )infoFile.folder[num],&gcodeRect[i]);
	}
	else if(num<infoFile.F_num+infoFile.f_num)
    {
        Scroll_CreatePara(&gcodeScroll, (u8* )infoFile.file[num-infoFile.F_num],&gcodeRect[i]);
	}
}

#define NAME_SPACE 110
#define NAME_LEN   (NAME_SPACE/BYTE_WIDTH)
void normalNameDisp(GUI_RECT *rect, u8 *name)
{
	u8 len,x_offset; 	
	
	if(name == NULL) return;
    
	len = my_strlen((u8 *)name);	
	if(len > NAME_LEN)
	{		
		len = NAME_LEN;
	}
	x_offset=((110 - (len * BYTE_WIDTH)) >> 1);	
    GUI_ClearRect(rect->x0, rect->y0, rect->x1, rect->y1);
    GUI_SetRange(rect->x0+x_offset, rect->y0, rect->x0+x_offset+len*BYTE_WIDTH, rect->y0+BYTE_HEIGHT);
	GUI_DispLenString(rect->x0+x_offset, rect->y0, (u8 *)name, 0, len);
    GUI_CancelRange();
}
void gocdeListDraw(void)
{
	u8 i=0;

	scrollFileNameCreate(0);
	Scroll_CreatePara(&titleScroll, (u8* )infoFile.title,&titleRect);
    GUI_ClearRect(titleRect.x0, titleRect.y0, titleRect.x1, titleRect.y1);

	for(i=0;(i + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num)
             &&(i < NUM_PER_PAGE)                                  ; i++)				//folder
	{
        printItems.items[i].icon = ICON_FOLDER;
        menuDrawItem(&printItems.items[i], i);
		normalNameDisp(&gcodeRect[i], (u8* )infoFile.folder[i + infoFile.cur_page * NUM_PER_PAGE]);
	}
	for(;(i + infoFile.cur_page * NUM_PER_PAGE < infoFile.f_num + infoFile.F_num)
          &&(i < NUM_PER_PAGE)                                                   ;i++)	//file
	{	
        printItems.items[i].icon = ICON_FILE;
        menuDrawItem(&printItems.items[i], i);
		normalNameDisp(&gcodeRect[i], (u8* )infoFile.file[i + infoFile.cur_page * NUM_PER_PAGE - infoFile.F_num]);
	}
	for(;(i<NUM_PER_PAGE);i++)			//file
	{		
        printItems.items[i].icon = ICON_BACKGROUND;
        menuDrawItem(&printItems.items[i], i);
	}
}

void menuPrint(void)
{
	KEY_VALUES key_num = KEY_IDLE;
    
	u8 update=0;
	
	GUI_Clear(BK_COLOR);
	GUI_DispString((LCD_WIDTH - my_strlen(textSelect(LABEL_LOADING))*BYTE_WIDTH)/2, 130, textSelect(LABEL_LOADING),1);
	
	if(mountSDCard()==true && scanPrintFiles() == true)
	{
        menuDrawPage(&printItems);
		gocdeListDraw();		
	}
	else
	{
		GUI_DispString((LCD_WIDTH-my_strlen(textSelect(LABEL_READ_SD_ERROR))*BYTE_WIDTH)/2, 160, textSelect(LABEL_READ_SD_ERROR),1);
		Delay_ms(1000);
		infoMenu.cur--;
	}

	while(infoMenu.menu[infoMenu.cur] == menuPrint)
	{
        Scroll_DispString(&titleScroll,1,LEFT);    //滚动显示路径名
		Scroll_DispString(&gcodeScroll,1,CENTER);  //滚动显示文件名
		
		key_num = menuKeyGetValue();
		
        
        switch(key_num)
		{
			case  KEY_ICON_5:			
				if(infoFile.cur_page > 0)
				{
					infoFile.cur_page--;
					update=1;
				}
				break;
				
			case KEY_ICON_6:	
				if(infoFile.cur_page+1 < (infoFile.F_num+infoFile.f_num+(NUM_PER_PAGE-1))/NUM_PER_PAGE)
				{
					infoFile.cur_page++;
					update=1;
				}	
				break;
				
			case KEY_ICON_7:
				infoFile.cur_page = 0;
				if(IsRootDir() == true)
				{
					clearInfoFile();
                    infoMenu.cur--;
					break;
				}
				else
				{
					ExitDir();				
					scanPrintFiles();			
					update = 1;
				}
				break;
				
			case KEY_IDLE:
				break;
			
			default:                
                if(key_num <= KEY_ICON_4)
                {	
                    u16 start = infoFile.cur_page * NUM_PER_PAGE;
                    if(key_num + start < infoFile.F_num)						//文件夹
                    {
                        if(EnterDir(infoFile.folder[key_num + start])==false)  break;						
                        scanPrintFiles();						
                        update=1;
                        infoFile.cur_page=0;		
                    }
                    else if(key_num+start<infoFile.F_num+infoFile.f_num)	//gcode文件
                    {	
                        if(infoHost.connected !=true) break;
                        if(EnterDir(infoFile.file[key_num + start - infoFile.F_num]) == false) break;	
                        
                        infoMenu.menu[++infoMenu.cur] = menuBeforePrinting;		                           
                    }				
                }
                else if(key_num >=KEY_LABEL_0 && key_num <= KEY_LABEL_4)
                {                  
                    if(key_num - KEY_LABEL_0 + infoFile.cur_page * NUM_PER_PAGE < infoFile.F_num + infoFile.f_num)
                    {
                        normalNameDisp(gcodeScroll.rect, gcodeScroll.text);
                        scrollFileNameCreate(key_num - KEY_LABEL_0);
                    }
                }	
                break;

		}
		if(update)
		{
			update=0;
			gocdeListDraw();
		}
		loopProcess();
	}
}




