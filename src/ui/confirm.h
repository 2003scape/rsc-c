#ifndef _H_CONFIRM
#define _H_CONFIRM

#include "../mudclient.h"

#define CONFIRM_DIALOG_WIDTH 300
#define CONFIRM_DIALOG_HEIGHT 80

/* size of cancel and OK buttons */
#define CONFIRM_BUTTON_SIZE 40

void mudclient_draw_confirm(mudclient *mud);

#endif
