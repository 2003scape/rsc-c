#ifndef _H_TRANSACTION
#define _H_TRANSACTION

#include "../mudclient.h"

#define TRANSACTION_IS_COMPACT(mud)                                            \
    (mud->surface->width < 490 || mud->surface->height < 294)

/* inventory items grid */
#define TRANSACTION_INVENTORY_ROWS 6
#define TRANSACTION_INVENTORY_COLUMNS 5

#define TRANSACTION_BUTTON_WIDTH 69
#define TRANSACTION_BUTTON_HEIGHT 21

/* for offer-x */
#define TRANSACTION_OFFER_OFFER 0
#define TRANSACTION_OFFER_REMOVE 1

void mudclient_offer_transaction_item(mudclient *mud,
                                      ClientOpcode update_opcode, int item_id,
                                      int item_amount);
void mudclient_remove_transaction_item(mudclient *mud,
                                       ClientOpcode update_opcode, int item_id,
                                       int item_amount);
void mudclient_draw_transaction_items(mudclient *mud, int x, int y, int rows,
                                      int *items, int *items_count,
                                      int item_count);
void mudclient_draw_transaction(mudclient *mud, int dialog_x, int dialog_y,
                                int is_trade);
void mudclient_draw_transaction_items_confirm(
    mudclient *mud, int x, int y, int *transaction_confirm_items,
    int *transaction_confirm_items_count, int transaction_confirm_item_count);
void mudclient_draw_transaction_confirm(mudclient *mud, int dialog_x,
                                        int dialog_y, int is_trade);

#endif
