#include "trade.h"

void mudclient_draw_trade(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - TRANSACTION_WIDTH / 2; // 22

    int dialog_y =
        (mud->surface->height / 2 - TRANSACTION_HEIGHT / 2) + 2; // 36

    mudclient_draw_transaction(mud, dialog_x, dialog_y, 1);
}

void mudclient_draw_trade_confirm(mudclient *mud) {
    int dialog_x = mud->surface->width / 2 - TRANSACTION_WIDTH / 2; // 22

    int dialog_y =
        (mud->surface->height / 2 - TRANSACTION_HEIGHT / 2) + 2; // 36

    mudclient_draw_transaction_confirm(mud, dialog_x, dialog_y, 1);
}
