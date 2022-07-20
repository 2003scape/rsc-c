#ifndef _H_TRADE
#define _H_TRADE

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"
#include "../surface.h"
#include "../utility.h"
#include "message-tabs.h"

#define TRADE_OFFER_ROWS 3
#define TRADE_OFFER_COLUMNS 4

/* must be 12 */
#define TRADE_OFFER_MAX (TRADE_OFFER_ROWS * TRADE_OFFER_COLUMNS)

void mudclient_draw_trade_items(mudclient *mud, int x, int y, int *trade_items,
                                int *trade_items_count, int trade_item_count);
void mudclient_draw_trade_items_confirm(mudclient *mud, int x, int y,
                                        int *trade_confirm_items,
                                        int *trade_confirm_items_count,
                                        int trade_confirm_item_count);
void mudclient_draw_trade(mudclient *mud);
void mudclient_draw_trade_confirm(mudclient *mud);

#endif
