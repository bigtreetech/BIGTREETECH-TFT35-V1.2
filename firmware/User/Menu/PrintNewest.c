#include "PrintNewest.h"
#include "includes.h"



//确认打印最新gcode界面
void menuEnsurePrintNewestGcode(void)
{
	u16 key_num=NO_TOUCH;
	if(mountSDCard()!=true || Get_NewestGcode("SD:")==false)
	{
    infoMenu.cur--;
		return;
	}
//	menuEnsurePic(textSelect(TEXT_PRINT_ENSURE),infoFile.title,textSelect(TEXT_PRINT_YES),textSelect(TEXT_PRINT_NO));
	while(infoMenu.menu[infoMenu.cur]==menuEnsurePrintNewestGcode)
	{
		key_num=KEY_GetValue(TOTAL_ENSURE,menuRectEnsure);
		switch(key_num)
		{
			case ENSURE_YES:		
				infoMenu.menu[1]=menuPrint;
				infoMenu.menu[2]=menuPrinting;
				infoMenu.menu[3]=menuLeveling;
				infoMenu.cur=3;
			  break;
			case ENSURE_NO:
				resetInfoFile();
				infoMenu.cur--;
				break;				
		}
		if(isCardExist()==false)
		{
			resetInfoFile();
			infoMenu.cur--;
		}
		loopProcess();
	}
}




