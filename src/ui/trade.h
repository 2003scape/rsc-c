#ifndef _H_TRADE
#define _H_TRADE

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"
#include "../surface.h"
#include "../utility.h"
#include "message-tabs.h"

void mudclient_draw_trade(mudclient *mud);
void mudclient_draw_trade_confirm(mudclient *mud);

#endif
