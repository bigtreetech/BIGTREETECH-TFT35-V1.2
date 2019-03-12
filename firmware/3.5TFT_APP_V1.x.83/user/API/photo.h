#ifndef _PHOTO_H_
#define _PHOTO_H_
#include "stm32f10x.h"

#define ADDRESS(num)  ((num)*0x5000+0x4B000)

#define BYTE_ADDR			0x200000
#define WORD_ADDR			0x300000

#define UNI2OEM_ADDR	0x600000
#define OEM2UNI_ADDR	0x700000

enum
{
 HEAT_ICON =0	,
 MOVE_ICON				,
 HOME_ICON				,
 PRINT_ICON 			,
 EXTRUDE_ICON 		,
 FAN_ICON 				,
 SET_ICON 				,
 LEVELING_ICON  	,

 ADD_ICON 				,
 SUB_ICON 				,
 NOZZLE_ICON 			,
 BED_ICON  				,
 DEGREE_ICON_1  	,
 DEGREE_ICON_5  	,		
 DEGREE_ICON_10 	,
 STOP_ICON  			,
 BACK_ICON    		,

 XADD_ICON 				,
 YADD_ICON 				,
 ZADD_ICON 				,
 MMM_01_ICON 			,
 MMM_1_ICON				,
 MMM_10_ICON 			,
 XSUB_ICON  			,
 YSUB_ICON  			,
 ZSUB_ICON  			,

 ZERO_X_ICON 			,
 ZERO_Y_ICON  		,
 ZERO_Z_ICON  		,

 FOLDER_ICON  		,
 FILE_ICON  			,
 LAST_ICON  			,
 NEXT_ICON  			,

 PAUSE_ICON  			,
 RESUME_ICON 			,

 EADD_ICON  			,
 ESUB_ICON  			,
 SLOW_ICON  			,
 NORMAL_ICON  		,
 FAST_ICON  			,
 EMM_ICON_1  			,
 EMM_ICON_5  			,
 EMM_ICON_10  		,

 FULL_ICON  			,
 HALF_ICON  			,

 CLOSE_ICON  			,
 LANGUE_ICON  		,
 ADJUST_ICON 			,
 ABOUT_ICON  			,
 DISCONNECT_ICON	,
 RESET_ICON				,


 BLACK_NOZZLE 		,
 BLACK_BED		  	,
 SPEED_ICON				,
 RESERVE_1_ICON		,
 RESERVE_2_ICON		,
 RESERVE_3				,
 RESERVE_4				,
 RESERVE_5				,
 RESERVE_6				,
 RESERVE_7				,
 RESERVE_8				,
 RESERVE_9				,
 RESERVE_10				,
 RESERVE_11				,
 RESERVE_12				,
};
void W25QXX_ReadStart(uint32_t ReadAddr);
void W25QXX_ReadEnd(void);


void Save_Photo(u8 rank,const u8 *photo);
void Read_Display_Photo(u8 place,u8 rank,u8 flag);
void Save_Photos(u8 rank,u8 num, u8 *photo[]);
void Read_Dispaly_Logo(void);
#endif

