#ifndef _H_SHOP
#define _H_SHOP

#include "../colours.h"
#include "../mudclient.h"
#include "../client-opcodes.h"

#define SHOP_SLOT_WIDTH 49
#define SHOP_SLOT_HEIGHT 34

#define COINS_ID 10

void mudclient_draw_shop(mudclient *mud);

#endif
