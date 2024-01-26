#include "trade.h"

void mudclient_draw_trade(mudclient *mud) {
    int is_compact = TRANSACTION_IS_COMPACT(mud);

    int columns = is_compact ? 3 : 4;

    int slot_width = is_compact ? 39 : ITEM_GRID_SLOT_WIDTH;
    int slot_height = is_compact ? 27 : ITEM_GRID_SLOT_HEIGHT;

    int inventory_width = (TRANSACTION_INVENTORY_COLUMNS * slot_width);

    int transaction_width =
        inventory_width + (columns * slot_width) + (is_compact ? 8 : 27);

    int transaction_height =
        ((TRANSACTION_INVENTORY_ROWS * slot_height) + (is_compact ? 43 : 74));

    int dialog_x = mud->surface->width / 2 - transaction_width / 2; // 22

    int dialog_y = (mud->surface->height / 2 - transaction_height / 2) +
                   (is_compact ? 0 : 2); // 36

    mudclient_draw_transaction(mud, dialog_x, dialog_y, 1);
}

void mudclient_draw_trade_confirm(mudclient *mud) {
    int is_compact = TRANSACTION_IS_COMPACT(mud);

    int columns = is_compact ? 3 : 4;

    int slot_width = is_compact ? 39 : ITEM_GRID_SLOT_WIDTH;
    int slot_height = is_compact ? 27 : ITEM_GRID_SLOT_HEIGHT;

    int inventory_width = (TRANSACTION_INVENTORY_COLUMNS * slot_width);

    int transaction_width =
        inventory_width + (columns * slot_width) + (is_compact ? 8 : 27);

    int transaction_height =
        ((TRANSACTION_INVENTORY_ROWS * slot_height) + (is_compact ? 43 : 74));

    int dialog_x = mud->surface->width / 2 - transaction_width / 2; // 22

    int dialog_y = (mud->surface->height / 2 - transaction_height / 2) +
                   (is_compact ? -8 : 2); // 36

    mudclient_draw_transaction_confirm(mud, dialog_x, dialog_y, 1);
}
