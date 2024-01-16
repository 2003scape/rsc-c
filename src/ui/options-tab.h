#ifndef _H_OPTIONS_TAB
#define _H_OPTIONS_TAB

typedef enum {
    PASSWORD_STEP_NONE,
    PASSWORD_STEP_NEW,
    PASSWORD_STEP_CONFIRM,
    PASSWORD_STEP_MISMATCH,
    PASSWORD_STEP_FINISHED,
    PASSWORD_STEP_SHORT,
    PASSWORD_STEP_CURRENT
} ChangePasswordStep;

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"
#include "../packet-stream.h"
#include "../surface.h"
#include "confirm.h"

#define CHANGE_PASSWORD_HEIGHT 60
#define CHANGE_PASSWORD_WIDTH 300

#define OPTIONS_WIDTH 196
#define OPTIONS_LINE_BREAK 15

void mudclient_send_privacy_settings(mudclient *mud, int chat, int private_chat,
                                     int trade, int duel);
void mudclient_draw_change_password(mudclient *mud);
void mudclient_draw_ui_tab_options(mudclient *mud, int no_menus);

#endif
