#ifndef _H_SOCIAL_TAB
#define _H_SOCIAL_TAB

#include "../client-opcodes.h"
#include "../colours.h"
#include "../mudclient.h"
#include "../packet-stream.h"
#include "../utility.h"

#define SOCIAL_WIDTH 196
#define SOCIAL_HEIGHT 182
#define SOCIAL_TAB_HEIGHT 24

#define SOCIAL_TAB_SPRITE_OFFSET 5

#ifdef REVISION_177
#define FRIEND_ONLINE 99
#else
#define FRIEND_ONLINE 255
#endif

extern char *social_tabs[];

void mudclient_sort_friends(mudclient *mud);
void mudclient_add_friend(mudclient *mud, char *username);
void mudclient_remove_friend(mudclient *mud, int64_t encoded_username);
void mudclient_add_ignore(mudclient *mud, char *username);
void mudclient_remove_ignore(mudclient *mud, int64_t encoded_username);
void mudclient_send_private_message(mudclient *mud, int64_t username,
                                    int8_t *message, int length);

void mudclient_draw_ui_tab_social(mudclient *mud, int no_menus);
void mudclient_draw_social_input(mudclient *mud);

#endif
