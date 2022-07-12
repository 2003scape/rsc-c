#ifndef _H_SHOP
#define _H_SHOP

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"

#define SHOP_SLOT_WIDTH 49
#define SHOP_SLOT_HEIGHT 34

#define SHOP_COLUMNS 8
#define SHOP_ROWS 5

#define SHOP_WIDTH ((SHOP_SLOT_WIDTH * SHOP_COLUMNS) + 16)
#define SHOP_HEIGHT ((SHOP_SLOT_HEIGHT * SHOP_ROWS) + 76)

#define COINS_ID 10

void mudclient_draw_shop(mudclient *mud);

#endif
