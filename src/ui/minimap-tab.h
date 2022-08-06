#ifndef _H_MINIMAP_TAB
#define _H_MINIMAP_TAB

#include "../mudclient.h"

#define MINIMAP_WIDTH 156
#define MINIMAP_HEIGHT 152

#define MINIMAP_TAB_SPRITE_OFFSET 2

void mudclient_draw_minimap_entity(mudclient *mud, int x, int y, int colour);
void mudclient_draw_ui_tab_minimap(mudclient *mud, int no_menus);

#endif
