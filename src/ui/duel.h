#ifndef _H_DUEL
#define _H_DUEL

#include "../mudclient.h"
#include "transaction.h"

/* yours and opponents items offer grid */
#define DUEL_OFFER_ROWS 2
#define DUEL_OFFER_HEIGHT (DUEL_OFFER_ROWS * ITEM_GRID_SLOT_HEIGHT) // 68

/* must be 8 */
#define DUEL_OFFER_MAX (DUEL_OFFER_ROWS * TRANSACTION_OFFER_COLUMNS)

void mudclient_draw_duel(mudclient *mud);
void mudclient_draw_duel_confirm(mudclient *mud);

#endif
