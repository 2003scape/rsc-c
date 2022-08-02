#ifndef _H_TRADE
#define _H_TRADE

#include "../mudclient.h"
#include "transaction.h"

/* yours and opponents items offer grid */
#define TRADE_OFFER_ROWS 3
#define TRADE_OFFER_HEIGHT (TRADE_OFFER_ROWS * ITEM_GRID_SLOT_HEIGHT)  // 102

/* must be 12 */
#define TRADE_OFFER_MAX (TRADE_OFFER_ROWS * TRANSACTION_OFFER_COLUMNS)

void mudclient_draw_trade(mudclient *mud);
void mudclient_draw_trade_confirm(mudclient *mud);

#endif
