#ifndef _H_ADDITIONAL_OPTIONS
#define _H_ADDITIONAL_OPTIONS

#include "../mudclient.h"

#define ADDITIONAL_OPTIONS_WIDTH 280
#define ADDITIONAL_OPTIONS_HEIGHT 180

#define OPTIONS_TAB_HEIGHT 24

extern char *option_tabs[];

void mudclient_create_options_panel(mudclient *mud);
void mudclient_draw_additional_options(mudclient *mud);
void mudclient_handle_additional_options_input(mudclient *mud);

#endif
