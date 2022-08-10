#ifndef _H_STATUS_BARS
#define _H_STATUS_BARS

#include "../mudclient.h"

#define STATUS_BAR_WIDTH ((COMBAT_STYLE_WIDTH / 2) - 1)
#define STATUS_BAR_HEIGHT 16

void mudclient_draw_status_bars(mudclient *mud);

#endif
