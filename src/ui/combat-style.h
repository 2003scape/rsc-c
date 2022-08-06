#ifndef _H_COMBAT_STYLE
#define _H_COMBAT_STYLE

#include "../mudclient.h"

#define COMBAT_BUTTON_HEIGHT 20
#define COMBAT_STYLE_WIDTH 175

extern char *combat_styles[];

void mudclient_draw_combat_style(mudclient *mud);

#endif
