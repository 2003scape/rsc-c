#ifndef _H_SERVER_MESSAGE
#define _H_SERVER_MESSAGE

#include "../mudclient.h"

#define SERVER_MESSAGE_WIDTH (MUD_IS_COMPACT ? MUD_WIDTH : 400)

void mudclient_draw_server_message(mudclient* mud);

#endif
