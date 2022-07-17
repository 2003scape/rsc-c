#ifndef _H_BANK
#define _H_BANK

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"
#include "../packet-stream.h"
#include "../surface.h"

#define BANK_WIDTH 408

#define BANK_COLUMNS 8
#define BANK_ROWS 6

#define BANK_PAGE_BUTTON_WIDTH 65

#define BANK_MAGIC_DEPOSIT 0x87654321
#define BANK_MAGIC_WITHDRAW 0x12345678

#define BANK_OFFER_WITHDRAW 1
#define BANK_OFFER_DEPOSIT 2

/* milliseconds before scrolling using the buttons */
#define BANK_SCROLL_SPEED 120

void mudclient_bank_transaction(mudclient *mud, int item_id, int amount,
                                int opcode);
void mudclient_add_bank_menu(mudclient *mud, int type, int item_id,
                             int amount, char *display_amount, char *item_name);
void mudclient_add_bank_menus(mudclient *mud, int type, int item_id,
                              int item_amount, char *item_name);
void mudclient_draw_bank_amounts(mudclient *mud, int amount, int last_x, int x,
                                 int y);
void mudclient_handle_bank_amounts_input(mudclient *mud, int amount, int last_x,
                                    int x, int y, int transaction_opcode);
void mudclient_draw_bank(mudclient *mud);

#endif
