#include "extrude.h"
#include "includes.h"

//1个title(标题), ITEM_PER_PAGE个item(图标+标签) 
MENUITEMS extrudeItems = {
//   title
     LABEL_EXTRUDE,
//   icon                       label
   {{ICON_UNLOAD,               LABEL_UNLOAD},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_LOAD,                 LABEL_LOAD},
    {ICON_NOZZLE,               LABEL_NOZZLE},
    {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
    {ICON_E_5_MM,               LABEL_5_MM},
    {ICON_BACK,                 LABEL_BACK},}
};

#define ITEM_SPEED_NUM 3
const ITEM itemSpeed[ITEM_SPEED_NUM] = {
//   icon                       label
    {ICON_SLOW_SPEED,           LABEL_SLOW_SPEED},
    {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
    {ICON_FAST_SPEED,           LABEL_FAST_SPEED},
};
const  u32 item_speed[ITEM_SPEED_NUM] = {EXTRUDE_SLOW_SPEED, EXTRUDE_NORMAL_SPEED, EXTRUDE_FAST_SPEED};
static u8  item_speed_i = 1;



#define ITEM_LEN_NUM 3
const ITEM itemLen[ITEM_LEN_NUM] = {
//   icon                       label
    {ICON_E_1_MM,               LABEL_1_MM},
    {ICON_E_5_MM,               LABEL_5_MM},
    {ICON_E_10_MM,              LABEL_10_MM},
};
const  u8 item_len[ITEM_LEN_NUM] = {1, 5, 10};
static u8 item_len_i = 1;


static float extrudeCoordinate = 0.0f;

void extrudeCoordinateReDraw(void)
{
	GUI_DispFloat(132+7*12,100,extrudeCoordinate,8,2);
}

void showExtrudeCoordinate(void)
{
	GUI_DispString(132,100,(u8 *)"E0(mm):",0);
	extrudeCoordinateReDraw();
}

void menuExtrude(void)
{
	
	KEY_VALUES key_num = KEY_IDLE;
    float eSaved = 0.0f;
    float eTemp  = 0.0f;
    u32   feedrate = 0;
//    bool  isExtrudeRelative = false;
      
    while(infoCmd.count != 0) {loopProcess();}
    extrudeCoordinate = eTemp = eSaved = coordinateGetAxis(E);                
    feedrate = coordinateGetFeedRate();
    
	menuDrawPage(&extrudeItems);
	showExtrudeCoordinate();
    
//    isExtrudeRelative = eGetRelative();
//    mustStoreCmd("M83\n");
//                
	while(infoMenu.menu[infoMenu.cur] == menuExtrude)
	{
        key_num = menuKeyGetValue();
        switch(key_num)
        {
			case KEY_ICON_0:
                eTemp -= item_len[item_len_i];
                break;
			case KEY_ICON_3:
                eTemp += item_len[item_len_i];
                break;
			case KEY_ICON_5:
				item_speed_i = (item_speed_i+1) % ITEM_SPEED_NUM;
                extrudeItems.items[key_num] = itemSpeed[item_speed_i];
                menuDrawItem(&extrudeItems.items[key_num], key_num);
                break;
			case KEY_ICON_6:
                item_len_i = (item_len_i+1) % ITEM_LEN_NUM;
                extrudeItems.items[key_num] = itemLen[item_len_i];
                menuDrawItem(&extrudeItems.items[key_num], key_num);
                break;
//                
			case KEY_ICON_7: infoMenu.cur--; break;
			default:break;            
        }	
        if(extrudeCoordinate != eTemp)
        {
            extrudeCoordinate = eTemp;
            extrudeCoordinateReDraw();
            storeCmd("G0 E%.3f F%d\n", extrudeCoordinate, item_speed[item_speed_i]);
        }
        loopProcess();
    }


//    if(isExtrudeRelative == false)
//    { 
//        mustStoreCmd("M82\n");
//    }  
    mustStoreCmd("G92 E%.3f\n",eSaved);   
    mustStoreCmd("G0 F%lu\n",feedrate);

}









