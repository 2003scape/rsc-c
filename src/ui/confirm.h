#ifndef _H_CONFIRM
#define _H_CONFIRM

typedef enum CONFIRM_TYPE {
    CONFIRM_TUTORIAL = 1,
    CONFIRM_OPTIONS_DEFAULT = 2,
    CONFIRM_OPTIONS_VANILLA = 3
} CONFIRM_TYPE;

#include "../mudclient.h"

#define CONFIRM_DIALOG_WIDTH 300
#define CONFIRM_DIALOG_HEIGHT 80

/* size of cancel and OK buttons */
#define CONFIRM_BUTTON_SIZE 40

void mudclient_draw_confirm(mudclient *mud);
void mudclient_handle_confirm_input(mudclient *mud);

#endif
