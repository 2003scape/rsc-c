#ifndef _H_SHOP
#define _H_SHOP

#include "../mudclient.h"

#define SHOP_COLUMNS 8
#define SHOP_ROWS 5

#define SHOP_WIDTH                                                             \
    ((ITEM_GRID_SLOT_WIDTH * SHOP_COLUMNS) + (MUD_IS_COMPACT ? 11 : 16))

#define SHOP_HEIGHT ((ITEM_GRID_SLOT_HEIGHT * SHOP_ROWS) + 76)

void mudclient_draw_shop(mudclient *mud);

#endif
