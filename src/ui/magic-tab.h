#ifndef _H_MAGIC_TAB
#define _H_MAGIC_TAB

#include "../mudclient.h"

#define MAGIC_HEIGHT 182
#define MAGIC_WIDTH 196
#define MAGIC_TAB_HEIGHT 24

extern char *magic_tabs[];

void mudclient_draw_ui_tab_magic(mudclient *mudclient, int no_menus);

#endif
