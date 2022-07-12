#ifndef _H_OFFER_X
#define _H_OFFER_X

#include "../mudclient.h"

#define OFFER_X_WIDTH 164
#define OFFER_X_HEIGHT 70

#define OFFER_X_CANCEL_SIZE 40

void mudclient_draw_offer_x(mudclient *mud);
void mudclient_handle_offer_x_input(mudclient *mud);

#endif
