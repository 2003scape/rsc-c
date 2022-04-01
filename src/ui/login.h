#ifndef _H_LOGIN
#define _H_LOGIN

#include "../mudclient.h"
#include "../panel.h"

void mudclient_create_login_panels(mudclient *mud);
void mudclient_show_login_screen_status(mudclient *mud, char *s, char *s1);
void mudclient_reset_login_screen(mudclient *mud);
void mudclient_render_login_screen_viewports(mudclient *mud);
void mudclient_draw_login_screens(mudclient *mud);
void mudclient_handle_login_screen_input(mudclient *mud);

#endif
