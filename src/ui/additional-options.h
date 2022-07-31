#ifndef _H_ADDITIONAL_OPTIONS
#define _H_ADDITIONAL_OPTIONS

#include "../mudclient.h"

#define ADDITIONAL_OPTIONS_WIDTH 348
#define ADDITIONAL_OPTIONS_HEIGHT 218

#define OPTIONS_TAB_HEIGHT 24

extern char *option_tabs[];

int mudclient_add_option_panel_label(Panel *panel, char *label, int x, int y);
int mudclient_add_option_panel_string(Panel *panel, char *label,
                                      char *default_text, int max_length, int x, int y);
void mudclient_create_options_panel(mudclient *mud);
Panel *mudclient_get_active_option_panel(mudclient *mud);
void mudclient_draw_additional_options(mudclient *mud);
void mudclient_handle_additional_options_input(mudclient *mud);

#endif
