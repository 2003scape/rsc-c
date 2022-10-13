#ifndef _H_APPEARANCE
#define _H_APPEARANCE

#include <string.h>

#include "../mudclient.h"

/* size of -> sprite */
#define APPEARANCE_ARROW_SIZE 20

/* box around type text */
#define APPEARANCE_BOX_PADDING (MUD_IS_COMPACT ? 5 : 8)

#define APPEARANCE_BOX_WIDTH 53
#define APPEARANCE_BOX_HEIGHT (APPEARANCE_BOX_PADDING + (MUD_IS_COMPACT ? 25 : 33))

#define APPEARANCE_BOX_MARGIN (APPEARANCE_BOX_HEIGHT + (MUD_IS_COMPACT ? 3 : 9))

/* the width of each option column including the arrows */
#define APPEARANCE_COLUMN_WIDTH 54

/* size of accept button */
#define APPEARANCE_ACCEPT_WIDTH 200
#define APPEARANCE_ACCEPT_HEIGHT 30

/* size of the character preview */
#define APPEARANCE_CHARACTER_WIDTH 64
#define APPEARANCE_CHARACTER_HEIGHT 102

struct appearance_buttons {
    int left;
    int right;
};

struct appearance_buttons
mudclient_create_appearance_box(mudclient *mud, char *type, int x, int y);

void mudclient_create_appearance_panel(mudclient *mud);
void mudclient_handle_appearance_panel_input(mudclient *mud);
void mudclient_draw_appearance_panel(mudclient *mud);

#endif
