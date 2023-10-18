#ifndef _H_COMBAT_STYLE
#define _H_COMBAT_STYLE

#include "../mudclient.h"

#define COMBAT_BUTTON_HEIGHT (MUD_IS_COMPACT ? 22 : 20)
#define COMBAT_STYLE_WIDTH (MUD_IS_COMPACT ? 106 : 175)

extern const char *combat_styles[];

void mudclient_draw_combat_style(mudclient *mud);

#endif
