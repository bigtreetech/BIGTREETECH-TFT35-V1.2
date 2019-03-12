/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sd.h"
/* Definitions of physical drive number for each drive */
#define SD_CARD	 0  //SD卡,卷标为0


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	switch (pdrv) {
		case SD_CARD:  stat=SD_Init();	break;
		default:			 stat=1;break;
	}
	if(stat)
		return STA_NOINIT;
	else
		return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 res;

	switch (pdrv) {
		case SD_CARD :
						res = SD_Read_Data(buff, sector,count);
						while(res)//读出错
						{
							SD_Init();	//重新初始化SD卡
							res=SD_Read_Data(buff,sector,count);	
						}
						break;
		default:
						res=1; 
	}
	if(res)return RES_ERROR;
	else	 return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u8 res;

	switch (pdrv) 
	{
			case SD_CARD :
				res = SD_Write_Data((u8 *)buff, sector, count);
				break;
			default:
				res=1; 
				break;
	}
	
	if(res == 0x00)return RES_OK;	 
	else return RES_ERROR;	
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	u8 result= RES_OK;
	switch (pdrv)
	{
		case SD_CARD : 
				switch(cmd)
				{
					case CTRL_SYNC:
					SD_CS_L();
					if(SD_Wait_Ready())result = RES_ERROR;/*等待卡准备好*/
					SD_CS_H();
					break;
					case GET_SECTOR_SIZE:
					*(WORD*)buff = 512;/*sd 卡扇区字节数*/
					break;
					case GET_BLOCK_SIZE:
					*(WORD*)buff = 8;/*块大小*/
					break;
					case GET_SECTOR_COUNT:
					*(DWORD*)buff = SD_Get_Sector_Count();/*获取 SD 卡的总扇区数*/
					break;
					default: result = RES_PARERR;
					break;
				}
				break;
		
		default:		break;
	}
	if(!result)return RES_OK;
	return RES_PARERR;
}

#include "stdlib.h"
void *ff_memalloc(UINT msize)
{
	return malloc(msize);
}
void ff_memfree(void *mblock)
{
	free(mblock);
	mblock=NULL;
}
DWORD get_fattime (void)
{
	u32 date;
	date=(	((2016-1980)<<25)|(11<<21)|(22<<16)|(15<<11)|(21<<5)|(0)  );
	return date;
}

