#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "stdint.h"

typedef struct
{
  uint32_t baudrate;
  uint8_t  language;
}SETTINGS;

extern SETTINGS infoSettings;

void menuSettings(void);

#endif
