#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "includes.h"






enum
{
	HEAT=0,
  MOVE,
	HOME	,
	PRINT		,
	EXTRUDE	,
	FAN				,
	SETTING		,
	
	RESERVE_1=54,
	RESERVE_2,
	READY	,
	PRINTING		,
	BUSY			,
	NOTCONNECT			,
	LOADING						,
	NOCARD								,
	FILESYS_FAILED					,
	SCANFILE_FAILED					,
	GET_STAT_FAILED					,
	ERR_WARNNING					,
	FILE_OPEN_FAILED			,
	MIN_TEMP							,
	OUTAGE_INFO		,
	CONTINUE,
	CANCEL	,
	
};







extern u8 *en_str[];
extern u8 *cn_str[];
extern u8 *ru_str[];
extern u8 *jp_str[];
extern u8 *armenian_str[];


bool isRussia(const u8 *str);
bool isArmenian(const u8 *str);
u16 my_strlen(const u8 *str);
u8 * textSelect(u8 sel);

#endif

