#ifndef _H_LOGIN
#define _H_LOGIN

typedef enum {
    LOGIN_STAGE_WELCOME = 0,
    LOGIN_STAGE_NEW = 1,
    LOGIN_STAGE_EXISTING = 2,
    LOGIN_STAGE_REGISTER = 3, // TODO don't think we're using this
    LOGIN_STAGE_RECOVER = 4
} LOGIN_STAGE;

#include "../mudclient.h"

#define LOGIN_BACKGROUND_HEIGHT ((int)((200 / 512.0f) * (float)(MUD_WIDTH)))

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
