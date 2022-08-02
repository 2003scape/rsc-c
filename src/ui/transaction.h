#ifndef _H_TRANSACTION
#define _H_TRANSACTION

#include "../mudclient.h"

/* yours and opponents items offer grid */
#define TRANSACTION_OFFER_COLUMNS 4

#define TRANSACTION_OFFER_WIDTH                                                \
    (TRANSACTION_OFFER_COLUMNS * ITEM_GRID_SLOT_WIDTH) // 196

#define TRANSACTION_OFFER_X 8
#define TRANSACTION_OFFER_Y 30

/* inventory items grid */
#define TRANSACTION_INVENTORY_ROWS 6
#define TRANSACTION_INVENTORY_COLUMNS 5

#define TRANSACTION_INVENTORY_WIDTH                                            \
    (TRANSACTION_INVENTORY_COLUMNS * ITEM_GRID_SLOT_WIDTH) // 245

#define TRANSACTION_INVENTORY_HEIGHT                                           \
    (TRANSACTION_INVENTORY_ROWS * ITEM_GRID_SLOT_HEIGHT) // 204

#define TRANSACTION_INVENTORY_X (TRANSACTION_OFFER_WIDTH + 20) // 216
#define TRANSACTION_INVENTORY_Y TRANSACTION_OFFER_Y

/* ui size */
#define TRANSACTION_WIDTH                                                      \
    ((TRANSACTION_INVENTORY_COLUMNS * ITEM_GRID_SLOT_WIDTH) +                  \
     (TRANSACTION_OFFER_COLUMNS * ITEM_GRID_SLOT_WIDTH) + 27) // 468

#define TRANSACTION_HEIGHT                                                     \
    ((TRANSACTION_INVENTORY_ROWS * ITEM_GRID_SLOT_HEIGHT) + 74) // 278

/* accept and decline buttons */
#define TRANSACTION_ACCEPT_X (TRANSACTION_OFFER_WIDTH + 21) // 221

#define TRANSACTION_DECLINE_X                                                  \
    (TRANSACTION_WIDTH - TRANSACTION_BUTTON_WIDTH - 5) // 394

#define TRANSACTION_BUTTON_Y (TRANSACTION_INVENTORY_HEIGHT + 34) // 238

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
