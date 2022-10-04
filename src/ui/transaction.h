#ifndef _H_TRANSACTION
#define _H_TRANSACTION

#include "../mudclient.h"

/* yours and opponents items offer grid */
#define TRANSACTION_OFFER_COLUMNS (MUD_IS_COMPACT ? 3 : 4)

#define TRADE_SLOT_WIDTH (MUD_IS_COMPACT ? 39 : ITEM_GRID_SLOT_WIDTH)
#define TRADE_SLOT_HEIGHT (MUD_IS_COMPACT ? 27 : ITEM_GRID_SLOT_HEIGHT)

#define TRANSACTION_OFFER_WIDTH                                                \
    (TRANSACTION_OFFER_COLUMNS * TRADE_SLOT_WIDTH) // 196

#define TRANSACTION_OFFER_X (MUD_IS_COMPACT ? 2 : 8)
#define TRANSACTION_OFFER_Y (MUD_IS_COMPACT ? 37 : 30)

/* inventory items grid */
#define TRANSACTION_INVENTORY_ROWS 6
#define TRANSACTION_INVENTORY_COLUMNS 5

#define TRANSACTION_INVENTORY_WIDTH                                            \
    (TRANSACTION_INVENTORY_COLUMNS * TRADE_SLOT_WIDTH) // 245

#define TRANSACTION_INVENTORY_HEIGHT                                           \
    (TRANSACTION_INVENTORY_ROWS * TRADE_SLOT_HEIGHT) // 204

#define TRANSACTION_INVENTORY_X                                                \
    (TRANSACTION_OFFER_WIDTH + (MUD_IS_COMPACT ? 5 : 20)) // 216

#define TRANSACTION_INVENTORY_Y (MUD_IS_COMPACT ? 14 : TRANSACTION_OFFER_Y)

/* ui size */
#define TRANSACTION_WIDTH                                                      \
    ((TRANSACTION_INVENTORY_COLUMNS * TRADE_SLOT_WIDTH) +                      \
     (TRANSACTION_OFFER_COLUMNS * TRADE_SLOT_WIDTH) +                          \
     (MUD_IS_COMPACT ? 8 : 27)) // 468

#define TRANSACTION_HEIGHT                                                     \
    ((TRANSACTION_INVENTORY_ROWS * TRADE_SLOT_HEIGHT) +                        \
     (MUD_IS_COMPACT ? 43 : 74)) // 278

/* accept and decline buttons */
#define TRANSACTION_ACCEPT_X                                                   \
    (TRANSACTION_INVENTORY_X + (MUD_IS_COMPACT ? 0 : 1)) // 221

#define TRANSACTION_DECLINE_X                                                  \
    (TRANSACTION_WIDTH - TRANSACTION_BUTTON_WIDTH -                            \
     (MUD_IS_COMPACT ? 2 : 5)) // 394

#define TRANSACTION_BUTTON_Y                                                   \
    (TRANSACTION_INVENTORY_HEIGHT + TRANSACTION_INVENTORY_Y +                  \
     (MUD_IS_COMPACT ? 3 : 4)) // 238

#define TRANSACTION_BUTTON_WIDTH 69
#define TRANSACTION_BUTTON_HEIGHT 21

/* for offer-x */
#define TRANSACTION_OFFER_OFFER 0
#define TRANSACTION_OFFER_REMOVE 1

void mudclient_offer_transaction_item(mudclient *mud, int update_opcode,
                                      int item_id, int item_amount);
void mudclient_remove_transaction_item(mudclient *mud, int update_opcode,
                                       int item_id, int item_amount);
void mudclient_draw_transaction_items(mudclient *mud, int x, int y, int rows,
                                      int *transaction_items,
                                      int *transaction_items_count,
                                      int transaction_item_count);
void mudclient_draw_transaction(mudclient *mud, int dialog_x, int dialog_y,
                                int is_trade);
void mudclient_draw_transaction_items_confirm(
    mudclient *mud, int x, int y, int *transaction_confirm_items,
    int *transaction_confirm_items_count, int transaction_confirm_item_count);
void mudclient_draw_transaction_confirm(mudclient *mud, int dialog_x,
                                        int dialog_y, int is_trade);

#endif
