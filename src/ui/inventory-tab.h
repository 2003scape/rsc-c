#ifndef _H_INVENTORY_TAB
#define _H_INVENTORY_TAB

#include "../mudclient.h"

#define INVENTORY_COLUMNS (MUD_IS_COMPACT ? 6 : 5)
#define INVENTORY_ROWS (INVENTORY_ITEMS_MAX / INVENTORY_COLUMNS)

#define INVENTORY_WIDTH (ITEM_GRID_SLOT_WIDTH * INVENTORY_COLUMNS)
#define INVENTORY_HEIGHT (ITEM_GRID_SLOT_HEIGHT * INVENTORY_ROWS)

#define INVENTORY_TAB_SPRITE_OFFSET 1

void mudclient_draw_ui_tab_inventory(mudclient *mud, int no_menus);

#endif
