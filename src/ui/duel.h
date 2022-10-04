#ifndef _H_DUEL
#define _H_DUEL

#include "../mudclient.h"
#include "transaction.h"

/* yours and opponents items offer grid */
#define DUEL_OFFER_ROWS (MUD_IS_COMPACT ? 3 : 2)
#define DUEL_OFFER_HEIGHT (DUEL_OFFER_ROWS * TRADE_SLOT_HEIGHT) // 68

/* must be at least 8 */
#define DUEL_OFFER_MAX (DUEL_OFFER_ROWS * TRANSACTION_OFFER_COLUMNS)

#define DUEL_OPTIONS_WIDTH ((TRANSACTION_OFFER_COLUMNS * TRADE_SLOT_WIDTH) + 1)
#define DUEL_OPTIONS_HEIGHT ((DUEL_CHECKBOX_HEIGHT * 2) + 22)

#define DUEL_LEFT_OPTION_WIDTH (MUD_IS_COMPACT ? 49 : 85)
#define DUEL_RIGHT_OPTION_WIDTH (MUD_IS_COMPACT ? 44 : 81)

#define DUEL_CHECKBOX_WIDTH 11
#define DUEL_CHECKBOX_HEIGHT 11

void mudclient_draw_duel(mudclient *mud);
void mudclient_draw_duel_confirm(mudclient *mud);

#endif
