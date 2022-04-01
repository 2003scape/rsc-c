#ifndef _H_INVENTORY_TAB
#define  _H_INVENTORY_TAB

#include "../colours.h"
#include "../mudclient.h"

#define INVENTORY_SLOT_WIDTH 49
#define INVENTORY_SLOT_HEIGHT 34

#define INVENTORY_COLUMNS 5
#define INVENTORY_ROWS (INVENTORY_ITEMS_MAX / INVENTORY_COLUMNS)

#define INVENTORY_WIDTH (INVENTORY_SLOT_WIDTH * INVENTORY_COLUMNS)
#define INVENTORY_HEIGHT (INVENTORY_SLOT_HEIGHT * INVENTORY_ROWS)

#define INVENTORY_TAB_SPRITE_OFFSET 1

void mudclient_draw_ui_tab_inventory(mudclient *mud, int no_menus);

#endif
