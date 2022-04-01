#ifndef _H_WELCOME
#define _H_WELCOME

#include "../colours.h"
#include "../mudclient.h"
#include "../packet-stream.h"
#include "../utility.h"

#define WELCOME_WIDTH 400

void mudclient_draw_welcome(mudclient *mud);

#endif
