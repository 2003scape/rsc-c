#ifndef _H_MAGIC_TAB
#define _H_MAGIC_TAB

#include "../colours.h"
#include "../mudclient.h"
#include "../packet-stream.h"
#include "./message-tabs.h"

#define MAGIC_HEIGHT 182
#define MAGIC_WIDTH 196
#define MAGIC_TAB_HEIGHT 24

#define MAGIC_TAB_SPRITE_OFFSET 4

extern char *magic_tabs[];

void mudclient_draw_ui_tab_magic(mudclient *mudclient, int no_menus);

#endif
