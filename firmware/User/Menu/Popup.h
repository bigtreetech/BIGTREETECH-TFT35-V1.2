#ifndef _POPUP_H_
#define _POPUP_H_
#include "includes.h"

enum
{
    KEY_POPUP_CONFIRM = 0,
    KEY_POPUP_CANCEL,
};

extern WINDOW window;

void windowSetButton(const BUTTON *btn, u16 btnNum);
void windowReDrawButton(u8 positon, u8 is_press);
void popupSetContext(const u8 *title, const u8 *context, const u8 *yes, const u8 *no);
void menuPopup(void);

#endif
