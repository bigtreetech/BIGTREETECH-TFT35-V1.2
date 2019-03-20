#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "includes.h"

typedef struct
{
    u32 baudrate;
    u8  language;
}SETTINGS;

extern SETTINGS infoSettings;

void menuSettings(void);

#endif
