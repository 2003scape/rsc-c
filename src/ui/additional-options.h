#ifndef _H_ADDITIONAL_OPTIONS
#define _H_ADDITIONAL_OPTIONS

#include "../mudclient.h"

#define ADDITIONAL_OPTIONS_WIDTH 320
#define ADDITIONAL_OPTIONS_HEIGHT 224

/* tabs */
#define ADDITIONAL_OPTIONS_TAB_HEIGHT 24

#define ADDITIONAL_OPTIONS_CONNECTION 0
#define ADDITIONAL_OPTIONS_CONTROL 1
#define ADDITIONAL_OPTIONS_DISPLAY 2
#define ADDITIONAL_OPTIONS_BANK 3

/* types */
#define ADDITIONAL_OPTIONS_STRING 0
#define ADDITIONAL_OPTIONS_INT 1
#define ADDITIONAL_OPTIONS_CHECKBOX 2

extern const char *option_tabs[];

void mudclient_create_options_panel(mudclient *mud);
Panel *mudclient_get_active_option_panel(mudclient *mud);
void mudclient_get_active_options(mudclient *mud, void ***options,
                                  int **option_types);
void mudclient_sync_options_panel(Panel *panel, void **options,
                                  int *option_types);
void mudclient_sync_options_panels(mudclient *mud);
void mudclient_draw_additional_options(mudclient *mud);
void mudclient_handle_additional_options_input(mudclient *mud);

#endif
