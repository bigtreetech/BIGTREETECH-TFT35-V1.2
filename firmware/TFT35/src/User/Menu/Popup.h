#ifndef _POPUP_H_
#define _POPUP_H_

#include "GUI.h"


#define POPUP_RECT_WINDOW         {80, 50, 400, 270}

#define POPUP_RECT_SINGLE_CONFIRM {180, 210, 300, 260}

#define POPUP_RECT_DOUBLE_CONFIRM {90,  210, 210, 260}
#define POPUP_RECT_DOUBLE_CANCEL  {270, 210, 390, 260}

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};

extern BUTTON bottomSingleBtn;
extern BUTTON bottomDoubleBtn[];
extern WINDOW window;

void windowSetButton(const BUTTON *btn);
void windowReDrawButton(uint8_t positon, uint8_t is_press);
void popupDrawPage(BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no);
void menuPopup(void);

#endif
