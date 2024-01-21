#ifndef _H_LOGIN
#define _H_LOGIN

typedef enum {
    LOGIN_STAGE_WELCOME = 0,
    LOGIN_STAGE_NEW = 1,
    LOGIN_STAGE_EXISTING = 2,
    LOGIN_STAGE_RECOVER = 3
} LOGIN_STAGE;

#include "../mudclient.h"

#ifdef RENDER_3DS_GL
#define LOGIN_RENDER_DELAY 250
#endif

void mudclient_create_login_panels(mudclient *mud);
void mudclient_show_login_screen_status(mudclient *mud, char *s, char *s1);
void mudclient_reset_login_screen(mudclient *mud);
void mudclient_render_login_scene_sprites(mudclient *mud);
void mudclient_draw_login_screens(mudclient *mud);
void mudclient_handle_login_screen_input(mudclient *mud);

#endif
