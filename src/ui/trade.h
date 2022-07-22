#ifndef _H_TRADE
#define _H_TRADE

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"
#include "../surface.h"
#include "../utility.h"
#include "message-tabs.h"
#include "offer-x.h"

/* yours and opponents items offer grid */
#define TRADE_OFFER_ROWS 3
#define TRADE_OFFER_COLUMNS 4

#define TRADE_OFFER_WIDTH (TRADE_OFFER_COLUMNS * ITEM_GRID_SLOT_WIDTH) // 196
#define TRADE_OFFER_HEIGHT (TRADE_OFFER_ROWS * ITEM_GRID_SLOT_HEIGHT)  // 102

/* must be 12 */
#define TRADE_OFFER_MAX (TRADE_OFFER_ROWS * TRADE_OFFER_COLUMNS)

#define TRADE_OFFER_X 8
#define TRADE_OFFER_Y 30
#define TRADE_OPPONENT_OFFER_Y (TRADE_OFFER_Y + TRADE_OFFER_HEIGHT + 23)

/* inventory items grid */
#define TRADE_INVENTORY_ROWS 6
#define TRADE_INVENTORY_COLUMNS 5

#define TRADE_INVENTORY_WIDTH                                                  \
    (TRADE_INVENTORY_COLUMNS * ITEM_GRID_SLOT_WIDTH) // 245

#define TRADE_INVENTORY_HEIGHT                                                 \
    (TRADE_INVENTORY_ROWS * ITEM_GRID_SLOT_HEIGHT) // 204

#define TRADE_INVENTORY_X (TRADE_OFFER_WIDTH + 20) // 216
#define TRADE_INVENTORY_Y TRADE_OFFER_Y

/* ui size */
#define TRADE_WIDTH                                                            \
    ((TRADE_INVENTORY_COLUMNS * ITEM_GRID_SLOT_WIDTH) +                        \
     (TRADE_OFFER_COLUMNS * ITEM_GRID_SLOT_WIDTH) + 27) // 468

#define TRADE_HEIGHT                                                           \
    ((TRADE_INVENTORY_ROWS * ITEM_GRID_SLOT_HEIGHT) + 74) // 278

/* accept and decline buttons */
#define TRADE_ACCEPT_X (TRADE_OFFER_WIDTH + 17)                // 217
#define TRADE_DECLINE_X (TRADE_WIDTH - TRADE_BUTTON_WIDTH - 5) // 394
#define TRADE_BUTTON_Y (TRADE_INVENTORY_HEIGHT + 34)           // 238

#define TRADE_BUTTON_WIDTH 69
#define TRADE_BUTTON_HEIGHT 21

/* for offer-x */
#define TRADE_OFFER_OFFER 0
#define TRADE_OFFER_REMOVE 1

void mudclient_draw_trade_items(mudclient *mud, int x, int y, int *trade_items,
                                int *trade_items_count, int trade_item_count);
void mudclient_draw_trade_items_confirm(mudclient *mud, int x, int y,
                                        int *trade_confirm_items,
                                        int *trade_confirm_items_count,
                                        int trade_confirm_item_count);
void mudclient_offer_trade_item(mudclient *mud, int item_id, int item_amount);
void mudclient_remove_trade_item(mudclient *mud, int item_id, int item_amount);
void mudclient_draw_trade(mudclient *mud);
void mudclient_draw_trade_confirm(mudclient *mud);

#endif
