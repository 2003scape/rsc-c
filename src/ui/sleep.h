#ifndef _H_SLEEP

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../mudclient.h"
#include "../colours.h"

#include "message-tabs.h"

void mudclient_draw_sleep(mudclient *mud);
void mudclient_handle_sleep_input(mudclient *mud);

#endif
