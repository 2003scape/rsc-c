#ifndef _H_OFFER_X
#define _H_OFFER_X

#include "../mudclient.h"

#define OFFER_X_WIDTH 164
#define OFFER_X_HEIGHT 70

#define OFFER_X_CANCEL_SIZE 40

void mudclient_draw_offer_x(mudclient *mud);
void mudclient_handle_offer_x_input(mudclient *mud);
void mudclient_add_offer_menu(mudclient *mud, int type, int item_id, int amount,
                              char *display_amount, char *item_name);
void mudclient_add_offer_menus(mudclient *mud, char *type_string, int type, int item_id,
                              int item_amount, char *item_name, int last_x);

#endif
