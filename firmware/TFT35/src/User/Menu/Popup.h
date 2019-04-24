#ifndef _POPUP_H_
#define _POPUP_H_

#include "GUI.h"

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};

extern WINDOW window;

void windowSetButton(const BUTTON *btn, uint16_t btnNum);
void windowReDrawButton(uint8_t positon, uint8_t is_press);
void popupSetContext(const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no);
void menuPopup(void);

#endif
