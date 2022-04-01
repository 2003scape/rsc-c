#ifndef _H_BANK
#define _H_BANK

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"
#include "../packet-stream.h"
#include "../surface.h"

#define BANK_SLOT_WIDTH 49
#define BANK_SLOT_HEIGHT 34

#define BANK_WIDTH 408
#define BANK_HEIGHT 334

#define BANK_ITEMS_PER_PAGE 48

#define BANK_MAGIC_DEPOSIT 0x87654321
#define BANK_MAGIC_WITHDRAW 0x12345678

void mudclient_draw_bank(mudclient *mud);

#endif
